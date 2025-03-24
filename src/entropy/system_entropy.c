//export PATH="/mingw64/bin:$PATH"
//gcc system_entropy.c -o system_entropy.exe -lbcrypt -mrdrnd
// gcc system_entropy.c shake.c -o entropy_mixer -lbcrypt -lcrypto -mrdrnd

#include "entropy.h"

uint8_t buffer[MIXED_ENTROPY_SIZE];

void display(uint8_t *buffer, size_t len) {
    for(int i = 0; i < len; i++) {
        printf("%02x", buffer[i]);
    }

    printf("%d",sizeof(buffer));
    printf("\n");
}

int get_system_entropy(uint8_t *buffer, size_t len) {

    if(BCryptGenRandom(NULL,buffer, len, BCRYPT_USE_SYSTEM_PREFERRED_RNG)!=0){ 
        printf("Error: BcryptGenRandom failed\n");
        return -1;
    }
    else{
        //display(buffer, len);
        return 0;
    }
}


int get_CPU_entropy(uint8_t *buffer, size_t len) {
    #if defined(__x86_64__) || defined(_M_X64) || defined(__i386__)
   

    for (size_t i = 0; i + sizeof(uint32_t) <= len; i += sizeof(uint32_t)) {
        uint32_t val;
        int success = 0;

        for (int j = 0; j < 10; j++) {
            success = _rdrand32_step(&val);  // Use the RDRAND intrinsic

            if (success) {
                memcpy(buffer + i, &val, sizeof(val));
                //printf("Stored %u at buffer[%zu]\n", val, i);
                break;
            }
        }

        if (!success) {
            printf("Error: RDRAND failed\n");
            return -1;
        }
    }

    
    //display(buffer, len);
    return 0;

    #else
        printf("Error: RDRAND is not supported on this CPU\n");
        return -1;
    #endif
    //here
}


void save_to_file(uint8_t *buffer, size_t len, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }
    fwrite(buffer, 1, len, file);
    fclose(file);
    printf("Entropy saved to %s\n", filename);
}

void mix_entropy(uint8_t *output) {
    uint8_t system_entropy[ENTROPY_SIZE];
    uint8_t CPU_entropy[ENTROPY_SIZE];

    if (get_system_entropy(system_entropy, ENTROPY_SIZE) == 0) {
        printf("System entropy generated successfully\n");
    } else {
        printf("Error: System entropy generation failed\n");
        return;
    }

    if (get_CPU_entropy(CPU_entropy, ENTROPY_SIZE) == 0) {
        printf("CPU entropy generated successfully\n");
    } else {
        printf("Error: CPU entropy generation failed\n");
        return;
    
    }

    memcpy(output, system_entropy, ENTROPY_SIZE);
    memcpy(output + ENTROPY_SIZE, CPU_entropy, ENTROPY_SIZE);   
}

int main(){
    
    uint8_t latest[64];
    size_t len = 32;
    mix_entropy(buffer);
    save_to_file(buffer, len, "output.txt");
    display(latest,OUTPUT_SIZE);
    
    hash_mix_entropy(latest);
    printf("SHAKE-256 hashed entropy:\n");
    display(latest, MIXED_ENTROPY_SIZE);
    save_to_file(latest, OUTPUT_SIZE, "hashed_entropy.txt");
    printf("%d",sizeof(buffer));

    return 0;
}