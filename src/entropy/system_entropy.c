//gcc system_entropy.c -o system_entropy.exe -lbcrypt -mrdrnd

#include "entropy.h"

void display(uint8_t *buffer, size_t len) {
    for(int i = 0; i < len; i++) {
        printf("%02x", buffer[i]);
    }
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

int main(){
    
    uint8_t buffer [ENTROPY_SIZE];
    size_t len = 32;
    if(get_system_entropy(buffer, len) == 0){
        printf("Entropy generated successfully\n");
        save_to_file(buffer, len, "output.txt");
    }

    if(get_CPU_entropy(buffer, len) == 0){
        printf("Hardware Entropy generated successfully\n");
     

        if(get_system_entropy(buffer, len) == 0){
            printf("Entropy generated successfully\n");
            save_to_file(buffer, len, "output.txt");
        
        }
    }
  
    else{
        printf("Error: Entropy generation failed\n");
    }

    return 0;
}