//gcc overhead.c -o overhead.exe -lssl -lcrypto

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>

#define OUTPUT_SIZE 160000  // Number of bytes to generate
#define SEED_SIZE 32        // 256-bit seed
#define AES_BLOCK_SIZE 16
#define RSA_BITS 2048

// ---------------- AES-CTR DRBG ----------------
typedef struct {
    AES_KEY aes_key;
    uint8_t counter[AES_BLOCK_SIZE];
} AESCTR_DRBG;

void increment_counter(uint8_t *counter) {
    for (int i = AES_BLOCK_SIZE - 1; i >= 0; i--) {
        if (++counter[i]) break;
    }
}

void aesctr_drbg_init(AESCTR_DRBG *drbg, uint8_t *seed) {
    AES_set_encrypt_key(seed, 256, &drbg->aes_key);
    RAND_bytes(drbg->counter, AES_BLOCK_SIZE);  // Random counter
}

void aesctr_drbg_generate(AESCTR_DRBG *drbg, uint8_t *output, size_t size) {
    uint8_t buffer[AES_BLOCK_SIZE];

    for (size_t i = 0; i < size; i += AES_BLOCK_SIZE) {
        AES_encrypt(drbg->counter, buffer, &drbg->aes_key);
        size_t chunk = (size - i < AES_BLOCK_SIZE) ? (size - i) : AES_BLOCK_SIZE;
        memcpy(output + i, buffer, chunk);
        increment_counter(drbg->counter);
    }
}

// ---------------- RSA-based DRBG ----------------
void rsa_drbg_generate(uint8_t *output, size_t size, uint8_t *seed, RSA *rsa) {
    size_t max_input = RSA_size(rsa) - 42; // PKCS#1 padding
    size_t generated = 0;
    uint8_t *input = malloc(max_input);

    while (generated < size) {
        for (size_t i = 0; i < max_input; i++) {
            input[i] = seed[i % SEED_SIZE];
        }

        uint8_t encrypted[RSA_size(rsa)];
        int enc_len = RSA_public_encrypt(max_input, input, encrypted, rsa, RSA_PKCS1_PADDING);
        if (enc_len <= 0) {
            fprintf(stderr, "RSA encryption failed.\n");
            break;
        }

        size_t to_copy = (generated + enc_len > size) ? (size - generated) : enc_len;
        memcpy(output + generated, encrypted, to_copy);
        generated += to_copy;
    }

    free(input);
}

// ---------------- Benchmarking ----------------
double benchmark_aes(uint8_t *seed, uint8_t *output) {
    AESCTR_DRBG drbg;
    aesctr_drbg_init(&drbg, seed);

    clock_t start = clock();
    aesctr_drbg_generate(&drbg, output, OUTPUT_SIZE);
    clock_t end = clock();

    return (double)(end - start) / CLOCKS_PER_SEC;
}

double benchmark_rsa(uint8_t *seed, uint8_t *output) {
    RSA *rsa = RSA_new();
    BIGNUM *e = BN_new();
    BN_set_word(e, RSA_F4);
    RSA_generate_key_ex(rsa, RSA_BITS, e, NULL);

    clock_t start = clock();
    rsa_drbg_generate(output, OUTPUT_SIZE, seed, rsa);
    clock_t end = clock();

    RSA_free(rsa);
    BN_free(e);

    return (double)(end - start) / CLOCKS_PER_SEC;
}

// ---------------- Main ----------------
int main() {
    FILE *f = fopen("results.csv", "a");  // Append mode
    if (!f) {
        perror("Failed to open results.csv");
        return 1;
    }

    for (int i = 0; i < 10; i++) {  // Run 10 iterations
        uint8_t seed[SEED_SIZE];
        RAND_bytes(seed, SEED_SIZE);

        uint8_t aes_output[OUTPUT_SIZE];
        uint8_t rsa_output[OUTPUT_SIZE];

        double aes_time = benchmark_aes(seed, aes_output);
        double rsa_time = benchmark_rsa(seed, rsa_output);

        fprintf(f, "%d,%.8f,%.8f\n", i + 1, aes_time, rsa_time);
        printf("Iteration %d complete\n", i + 1);
    }

    fclose(f);
    printf("Benchmark results saved to results.csv\n");
    return 0;
}

