//gcc system_entropy.c -o system_entropy.exe -lbcrypt

#include<stdio.h>
#include<stdint.h>
#include<windows.h>
#include<bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

int get_entropy(uint8_t *buffer, size_t len) {

    if(BCryptGenRandom(NULL,buffer, len, BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0) {
        printf("Error: BcryptGenRandom failed\n");
        return -1;
    }
    else{
        return 0;
    }
}

void display(uint8_t *buffer, size_t len) {
    for(int i = 0; i < len; i++) {
        printf("%02x", buffer[i]);
    }
    printf("\n");
}

int main(){
    
    uint8_t buffer [256];
    size_t len = 256;
    if(get_entropy(buffer, len) == 0){
        printf("Entropy generated successfully\n");
        display(buffer, len);
    }
    else{
        printf("Error: Entropy generation failed\n");
    }

    return 0;
}