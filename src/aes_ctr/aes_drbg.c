#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

#define AES_BLOCK_SIZE 16  // AES block size (128 bits)
#define KEY_SIZE 32        // AES-256 key size (256 bits)

// Structure to hold DRBG state
typedef struct {
    AES_KEY aes_key;
    uint8_t counter[AES_BLOCK_SIZE];
} AESCTR_DRBG;

// Function to initialize DRBG
void aesctr_drbg_init(AESCTR_DRBG *drbg, uint8_t *seed) {
    AES_set_encrypt_key(seed, 256, &drbg->aes_key);  // Set AES-256 key
    RAND_bytes(drbg->counter, AES_BLOCK_SIZE);       // Generate random counter
}

// Function to generate pseudo-random bytes
void aesctr_drbg_generate(AESCTR_DRBG *drbg, uint8_t *output, size_t size) {
    uint8_t buffer[AES_BLOCK_SIZE];
    
    for (size_t i = 0; i < size; i += AES_BLOCK_SIZE) {
        AES_encrypt(drbg->counter, buffer, &drbg->aes_key);  // Encrypt counter
        
        size_t chunk = (size - i < AES_BLOCK_SIZE) ? (size - i) : AES_BLOCK_SIZE;
        memcpy(output + i, buffer, chunk);  // Copy encrypted output
        
        // Increment counter (Big-Endian)
        for (int j = AES_BLOCK_SIZE - 1; j >= 0; j--) {
            if (++drbg->counter[j]) break;
        }
    }
}

// Example Usage
int main() {
    uint8_t seed[KEY_SIZE] = {0};  // Example 256-bit SHAKE-256 seed (Replace with actual)
    AESCTR_DRBG drbg;

    aesctr_drbg_init(&drbg, seed);  // Initialize DRBG

    uint8_t random_bytes[64];  // Generate 64 bytes of randomness
    aesctr_drbg_generate(&drbg, random_bytes, sizeof(random_bytes));

    printf("Generated Random Bytes: ");
    for (size_t i = 0; i < sizeof(random_bytes); i++) {
        printf("%02x", random_bytes[i]);
    }
    printf("\n");

    return 0;
}
