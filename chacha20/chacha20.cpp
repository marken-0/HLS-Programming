#include "cc20.h"

void cc20_algo(hls::stream<axis_data> &input, hls::stream<axis_data> &output){
    //***************** Reading Input from Stream (Start) *****************//
    uint8_t key[32];
    uint8_t counter[4];
    uint8_t nonce[12];
    uint8_t plaintext[len];

    axis_data local_stream;

    for (int i = 0; i < 32; i++){
        local_stream = input.read();
        key[i] = local_stream.data;
    }

    local_stream = input.read();
    for (int i = 0; i < 4; i++){
        counter[i] = local_stream.data;
    }

    for (int i = 0; i < 3; i++){
        local_stream = input.read();
        nonce[i] = local_stream.data;
    }

    for (int i = 0; i < 16; i++){
        local_stream = input.read();
        plaintext[i] = local_stream.data;
    }
    //***************** Reading Input from Stream (End) *****************//

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
    }

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

    //***************** Writing Output to Stream (Start) *****************//
    axis_data local_write;
    for (int i = 0; i < len; i++){
    	local_write.data = cipher[i];
        if (i == len-1){
        	local_write.last = 1;
        }
        else local_write.last = 0;
        output.write(local_write);
    }
}
