#include <stdio.h>
#define ROTR(x,a) (x>>a) ^ (x<<(64-a))
typedef unsigned __int64 bit64;
bit64 x[5] = { 0 };
bit64 IV = 0x80400c0600000000, key[2] = { 0 }, nonce[2] = { 0 };
bit64 constants[12] = { 0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 0x78, 0x69, 0x5a, 0x4b };
bit64 plaintext[10] = { 0 }, ciphertext[10] = { 0 }, Tag[2] = { 0 };
void add_contants(int i) {	x[2] ^= constants[i]; }
void substitution() {
	bit64 t0, t1, t2, t3, t4;
	x[0] ^= x[4]; x[4] ^= x[3]; x[2] ^= x[1];
	t0 = x[0]; t1 = x[1]; t2 = x[2]; t3 = x[3]; t4 = x[4];
	t0 = ~t0; t1 = ~t1; t2 = ~t2; t3 = ~t3; t4 = ~t4;
	t0 &= x[1]; t1 &= x[2]; t2 &= x[3]; t3 &= x[4]; t4 &= x[0];
	x[0] ^= t1; x[1] ^= t2; x[2] ^= t3; x[3] ^= t4; x[4] ^= t0;
	x[1] ^= x[0]; x[0] ^= x[4]; x[3] ^= x[2]; x[2] = ~x[2];
}
void linear_layer() {
	x[0] = x[0] ^ ROTR(x[0], 19) ^ ROTR(x[0], 28);
	x[1] = x[1] ^ ROTR(x[1], 61) ^ ROTR(x[1], 39);
	x[2] = x[2] ^ ROTR(x[2],  1) ^ ROTR(x[2],  6);
	x[3] = x[3] ^ ROTR(x[3], 10) ^ ROTR(x[3], 17);
	x[4] = x[4] ^ ROTR(x[4],  7) ^ ROTR(x[4], 41);
}
void permutation(int i) {
	add_contants(i);
	substitution();
	linear_layer();
}
void print_state() {	for (int i = 0; i < 5; i++)  printf("%016I64x\n", x[i]); }
void print_ciphertext() { 
	for (int i = 0; i < sizeof(ciphertext) / 8; i++)  printf("%016I64x\n", ciphertext[i]);
}
void initialization() {
	x[0] = IV;	x[1] = key[0];		x[2] = key[1];	x[3] = nonce[1];	x[4] = nonce[1];
	for (int i = 0; i < 12; i++) permutation(i);
	x[3] ^= key[0];	x[4] ^= key[1];
}
void encryption() {
	for (int j = 0; j < sizeof(plaintext) / 8; j++) {
		x[0] = plaintext[j] ^ x[0];
		ciphertext[j] = x[0];
		for (int i = 0; i < 6; i++) permutation(i + 6);
	}
}
void finalization() {
	x[1] ^= key[0];		x[2] ^= key[1];
	for (int i = 0; i < 12; i++) permutation(i);
	x[3] ^= key[0];		x[4] ^= key[1];
	Tag[0] = x[3];		Tag[1] = x[4];
}
int main() {
	initialization();
	encryption();
	finalization();
	print_ciphertext();
	printf("Tag: %016I64x %016I64x\n", Tag[0], Tag[1]);
	return 1;
}