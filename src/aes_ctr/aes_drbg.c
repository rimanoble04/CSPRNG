//gcc aes_drbg.c -o aes_drbg.exe -I"C:/msys64/mingw64/include" -L"C:/msys64/mingw64/lib" -lssl -lcrypto

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include "../entropy/entropy.h"

#define AES_BLOCK_SIZE 16
#define KEY_SIZE 32
#define RESEED_INTERVAL_LIMIT 3  // Set low for testing

// DRBG State
typedef struct {
    AES_KEY aes_key;
    uint8_t counter[AES_BLOCK_SIZE];
    uint64_t reseed_counter;
} AESCTR_DRBG;

// Print seed in hex
void print_seed_hex(const char *label, uint8_t *seed, size_t size) {
    printf("%s: ", label);
    for (size_t i = 0; i < size; i++) {
        printf("%02x", seed[i]);
    }
    printf("\n");
}

// DRBG init
void aesctr_drbg_init(AESCTR_DRBG *drbg, uint8_t *seed) {
    AES_set_encrypt_key(seed, 256, &drbg->aes_key);
    RAND_bytes(drbg->counter, AES_BLOCK_SIZE);
    drbg->reseed_counter = 0;
}

// Read seed from file
int read_seed_from_file(const char *filename, uint8_t *seed, size_t seed_size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }
    size_t bytes_read = fread(seed, 1, seed_size, file);
    fclose(file);
    if (bytes_read != seed_size) {
        fprintf(stderr, "Failed to read the expected number of bytes from file\n");
        return -1;
    }
    return 0;
}

// Generate random output
void aesctr_drbg_generate(AESCTR_DRBG *drbg, uint8_t *output, size_t size) {
    uint8_t buffer[AES_BLOCK_SIZE];
    clock_t start = clock();  // Start timing

    for (size_t i = 0; i < size; i += AES_BLOCK_SIZE) {
        if (drbg->reseed_counter >= RESEED_INTERVAL_LIMIT) {
            printf("\n---Reseed triggered after %llu blocks...\n", drbg->reseed_counter);

            int result = system("make -C ../entropy run");
            if (result != 0) {
                fprintf(stderr, "Error: Failed to execute entropy Makefile\n\n");
                exit(EXIT_FAILURE);
            }

            uint8_t new_seed[KEY_SIZE];
            if (read_seed_from_file("../entropy/key.bin", new_seed, KEY_SIZE) != 0) {
                fprintf(stderr, "Error: Failed to read new seed from key.bin\n");
                exit(EXIT_FAILURE);
            }

            print_seed_hex("\n---New Seed (Post-Reseed)", new_seed, KEY_SIZE);

            aesctr_drbg_init(drbg, new_seed);
            printf("\nDRBG successfully reseeded!\n\n");
        }

        AES_encrypt(drbg->counter, buffer, &drbg->aes_key);

        size_t chunk = (size - i < AES_BLOCK_SIZE) ? (size - i) : AES_BLOCK_SIZE;
        memcpy(output + i, buffer, chunk);

        for (int j = AES_BLOCK_SIZE - 1; j >= 0; j--) {
            if (++drbg->counter[j]) break;
        }

        drbg->reseed_counter++;
    }

    clock_t end = clock();  // End timing
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    FILE *f = fopen("../aes_ctr/aes_time.txt", "w");
    if (f) {
        fprintf(f, "%.8f", elapsed);
        fclose(f);
    }
}

// Save bitstream
void save_to_file(uint8_t *buffer, size_t len, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }
    for (size_t i = 0; i < len; ++i) {
        for (int bit = 7; bit >= 0; --bit) {
            fputc((buffer[i] >> bit) & 1 ? '1' : '0', file);
        }
    }
    fclose(file);
    printf("Bitstream saved to %s\n", filename);
}

// Main
int main() {
    uint8_t seed[KEY_SIZE];
    if (read_seed_from_file("../entropy/key.bin", seed, sizeof(seed)) != 0) {
        return 1;
    }

    print_seed_hex("---Initial Seed", seed, KEY_SIZE);

    AESCTR_DRBG drbg;
    aesctr_drbg_init(&drbg, seed);

    int output_size;
    printf("Enter the number of bytes to generate: ");
    scanf("%d", &output_size);

    uint8_t random_bytes[output_size];
    aesctr_drbg_generate(&drbg, random_bytes, sizeof(random_bytes));

    printf("\nGenerated Random Bytes (Hex): \n");
    for (size_t i = 0; i < sizeof(random_bytes); i++) {
        printf("%02x", random_bytes[i]);
    }
    printf("\n");

    save_to_file(random_bytes, sizeof(random_bytes), "../entropy/nist/sts-2.1.2/experiments/AlgorithmTesting/frequency.data");

    return 0;
}
