#ifndef AES_H
#define AES_H

#include <string.h>
#include <stdint.h>  // For uint8_t _t and uint32_t types
#include <stdio.h>   // For file operations
#include <stdlib.h>


#define DEC_VERIFIED			1
#define DEC_NOT_VERIFIED	0


// Function to initialize the decryption key (expanded from the user-supplied key).
void AES_init_ctx_dec(uint8_t* key);

// Function to decrypt a single block of 16 bytes of ciphertext.
void AES_decrypt_block(uint8_t* ciphertext, uint8_t* plaintext);

// Function to decrypt a file
int AES_decrypt_file(const char* input_file_path, const char* output_file_path);

uint32_t SubWord(uint32_t word);

uint32_t RotWord(uint32_t word);

void KeyExpansion(uint8_t* key);

void AES_init_ctx_dec(uint8_t* key);

static void InvSubBytes(uint8_t* state);

static void InvShiftRows(uint8_t* state);

void ReverseBytesInWords(void);

uint8_t xtime(uint8_t x);

uint8_t multiply(uint8_t x, uint8_t y);

void InvMixCol(uint8_t *state);

void AddRoundKey(uint8_t* state, int round);

void AES_decrypt_block(uint8_t* ciphertext, uint8_t* plaintext);

int AES_decrypt_file(const char* input_file_path, const char* output_file_path);

int read_file_to_buffer(const char* filename, uint8_t** buffer, size_t* size);

int AES_decrypt_buffer(const uint8_t* input_buffer, size_t input_size, uint8_t* output_buffer);

#endif // AES_H