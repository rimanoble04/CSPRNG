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

// Function to read seed from a binary file
int read_seed_from_file(const char *filename, uint8_t *seed, size_t seed_size) {
    FILE *file = fopen(filename, "rb");  // Open the file in binary mode
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    size_t bytes_read = fread(seed, 1, seed_size, file);  // Read the seed into the buffer
    fclose(file);

    if (bytes_read != seed_size) {
        fprintf(stderr, "Failed to read the expected number of bytes from file\n");
        return -1;
    }

    return 0;
}

// Example Usage
int main() {
    uint8_t seed[KEY_SIZE];  // Seed buffer (256 bits)
    
    // Read seed from key.bin
    if (read_seed_from_file("C:/Users/hngay/OneDrive/Desktop/Gouri HN/CSPRNG/CSPRNG/src/entropy/key.bin", seed, sizeof(seed)) != 0) {
        return 1;  // Error reading seed
    }

    AESCTR_DRBG drbg;
    aesctr_drbg_init(&drbg, seed);  // Initialize DRBG with the seed from file

    int output_size;
    printf("Enter the number of bytes to generate: ");
    scanf("%d", &output_size);  // Get the number of bytes to generate
    
    uint8_t random_bytes[output_size];  // Generate 64 bytes of randomness
    aesctr_drbg_generate(&drbg, random_bytes, sizeof(random_bytes));

    printf("Generated Random Bytes: ");
    for (size_t i = 0; i < sizeof(random_bytes); i++) {
        printf("%02x", random_bytes[i]);
    }
    printf("\n");

    return 0;
}
