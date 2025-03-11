#ifndef ENTROPY_H
#define ENTROPY_H

#include<stdio.h>
#include<stdint.h>
#include<windows.h>
#include<bcrypt.h>
#include <string.h>
#include <immintrin.h>  // Required for RDRAND intrinsic
#pragma comment(lib, "bcrypt.lib")

#define ENTROPY_SIZE 32

int get_system_entropy(uint8_t *buffer, size_t len);
int get_CPU_entropy(uint8_t *buffer, size_t len);
void save_to_file(uint8_t *buffer, size_t len, const char *filename);

#endif