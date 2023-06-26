#include "cc20.h" 
#include <stdio.h>
#include <stdint.h>

void cc20_algo_sw(uint8_t key[32], uint8_t counter[4], uint8_t nonce[12], uint8_t plaintext[len]){

    //***************** Initializing State Matrix (Start) *****************//
    uint32_t matrix[16], block[16];
    uint32_t constant[4] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574}; //Constants given in the algorithm

    matrix[0] = constant[0]; matrix[1] = constant[1]; matrix[2] = constant[2]; matrix[3] = constant[3]; // Constant

    for (int i = 0; i < 8; i++){
        matrix[4+i] = ((uint32_t)key[4*i] << 24) | ((uint32_t)key[4*i + 1] << 16) | ((uint32_t)key[4*i + 2] << 8) | ((uint32_t)key[4*i + 3]);
    }

    matrix[12] =  counter[3] << 24 | counter[2] << 16 | counter[1] << 8 | counter[0]; // Counter

    for (int i = 0; i < 3; i++){
        matrix[13+i] = ((uint32_t)nonce[4*i] << 24) | ((uint32_t)nonce[4*i + 1] << 16) | ((uint32_t)nonce[4*i + 2] << 8) | ((uint32_t)nonce[4*i + 3]);
    }
    //***************** Initializing State Matrix (End) *****************//

    //***************** Generating Cipher (Start) *****************//
    uint8_t cipher[len];
    uint8_t keystream[64];
    int num_blocks = len/64;
    int rem = len%64;

    for (int i = 0; i < num_blocks; i++){
        chacha20_block(matrix, block);
        for (int j = 0; j < 16; ++j) {
            keystream[4 * j] = block[j] >> 24;
            keystream[4 * j + 1] = block[j] >> 16;
            keystream[4 * j + 2] = block[j] >> 8;
            keystream[4 * j + 3] = block[j];
        }

        for (int j = 0; j < 64; j++){
            cipher[i*64 + j] = keystream[j] ^ plaintext[i*64 + j];
        }
        matrix[12]++;
        for (int i = 0; i < len; i++){
                printf("%d", keystream[i]);
        }
    }
    printf("\n");

    if (rem != 0){
        chacha20_block(matrix, block);
        for (int j = 0; j < 16; ++j) {
            keystream[4 * j] = block[j] >> 24;
            keystream[4 * j + 1] = block[j] >> 16;
            keystream[4 * j + 2] = block[j] >> 8;
            keystream[4 * j + 3] = block[j];
        }

        for (int j = 0; j < rem; j++){
            cipher[num_blocks*64 + j] = keystream[j] ^ plaintext[num_blocks*64 + j];
        }
    }
    //***************** Generating Cipher (End) *****************//
    for (int i = 0; i < len; i++){
        printf("%d", cipher[i]);
    }
}


int main(){
    hls::stream<axis_data> input, output;
    axis_data local_stream;


    uint8_t key1[32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x01};
    uint8_t counter1[4] = {0x00, 0x00, 0x00, 0x01};
    uint8_t nonce1[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    uint8_t plaintext1[64] = {0x41, 0x6e, 0x79, 0x20, 0x73, 0x75,
                                0x62, 0x6d, 0x69, 0x73, 0x73, 0x69,
                                0x6f, 0x6e, 0x20, 0x74, 0x6f, 0x20,
                                0x74, 0x68, 0x65, 0x20, 0x49, 0x45,
                                0x54, 0x46, 0x20, 0x69, 0x6e, 0x74,
                                0x65, 0x6e, 0x64, 0x65, 0x64, 0x20,
                                0x62, 0x79, 0x20, 0x74, 0x68, 0x65,
                                0x20, 0x43, 0x6f, 0x6e, 0x74, 0x72,
                                0x69, 0x62, 0x75, 0x74, 0x6f, 0x72,
                                0x20, 0x66, 0x6f, 0x72, 0x20, 0x70,
                                0x75, 0x62, 0x6c, 0x69};

    cc20_algo_sw(key1, counter1, nonce1, plaintext1);
}
