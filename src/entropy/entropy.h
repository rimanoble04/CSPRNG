#ifndef ENTROPY_H
#define ENTROPY_H

#include<stdio.h>
#include<stdint.h>
#include<windows.h>
#include<bcrypt.h>
#include<string.h>
#include<openssl/evp.h> // Required for SHAKE-256
#include<immintrin.h>  // Required for RDRAND intrinsic

#pragma comment(lib, "bcrypt.lib")
#define ENTROPY_SIZE 32
     #define OUTPUT_SIZE 64 
   #define MIXED_ENTROPY_SIZE 64  // 512-bit mixed entropy size

extern uint8_t buffer[MIXED_ENTROPY_SIZE];  // Global buffer (only declaration)


int get_system_entropy(uint8_t *buffer, size_t len);
int get_CPU_entropy(uint8_t *buffer, size_t len);
void mix_entropy(uint8_t *output);
void save_to_file(uint8_t *buffer, size_t len, const char *filename);
void shake256_mixing(uint8_t *input, size_t input_len, uint8_t *output, size_t output_len);
void hash_mix_entropy(uint8_t *output);

#endif