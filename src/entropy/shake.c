#include "entropy.h"

#define OUTPUT_SIZE 64  // SHAKE-256 output size (512-bit)




void shake256_mixing(uint8_t *input, size_t input_len, uint8_t *output, size_t output_len) {
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();  // Create SHAKE-256 context

    if (!mdctx) {
        printf("Error: Failed to create SHAKE-256 context\n");
        return;
    }

    // Initialize SHAKE-256
    EVP_DigestInit_ex(mdctx, EVP_shake256(), NULL);

    // Absorb input entropy
    EVP_DigestUpdate(mdctx, input, input_len);

    // Extract output
    EVP_DigestFinalXOF(mdctx, output, output_len);

    // Free the context
    EVP_MD_CTX_free(mdctx);
}

void hash_mix_entropy(uint8_t *output) {

    shake256_mixing(buffer, MIXED_ENTROPY_SIZE, output, OUTPUT_SIZE);    
}