#include "header.h"

#define leftShift(v, n) ( v<<n | v>>(32-n))

#define quarterRound(x, a, b, c, d) \
  x[a] += x[b]; x[d] = leftShift(x[d] ^ x[a], 16); \
  x[c] += x[d]; x[b] = leftShift(x[b] ^ x[c], 12); \
  x[a] += x[b]; x[d] = leftShift(x[d] ^ x[a], 8); \
  x[c] += x[d]; x[b] = leftShift(x[b] ^ x[c], 7);

void chacha20_block(uint32_t state[16], uint32_t out[16]) {
    for (int i = 0; i < 16; ++i) out[i] = state[i];
    for (int i = 0; i < 10; i ++) {
        quarterRound(out, 0, 4, 8, 12)
        quarterRound(out, 1, 5, 9, 13)
        quarterRound(out, 2, 6, 10, 14)
        quarterRound(out, 3, 7, 11, 15)
        quarterRound(out, 0, 5, 10, 15)
        quarterRound(out, 1, 6, 11, 12)
        quarterRound(out, 2, 7, 8, 13)
        quarterRound(out, 3, 4, 9, 14)
    }
    for (i = 0; i < 16; ++i) out[i] += state[i];
}
void cc20_algo(hls::stream<axis_data> &input, hls::stream<axis_data> &output){
    //***************** Reading Input from Stream (Start) *****************//
    ap_int<8> key[32];
    ap_int<8> counter[4];
    ap_int<8> nonce[12];
    ap_int<8> plaintext[len];

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
    ap_int<32> matrix[16], block[16]
    ap_int<32> constant[4] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574}; //Constants given in the algorithm

    matrix[0] = constant[0]; matrix[1] = constant[1]; matrix[2] = constant[2]; matrix[3] = constant[3]; // Constant

    for (int i = 0; i < 8; i++){
        matrix[4+i] = ((ap_int<32>)key[4*i] << 24) | ((ap_int<32>)key[4*i + 1] << 16) | ((ap_int<32>)key[4*i + 2] << 8) | ((ap_int<32>)key[4*i + 3]);
    }

    matrix[12] =  counter[3] << 24 | counter[2] << 16 | counter[1] << 8 | counter[0]; // Counter
    
    for (int i = 0; i < 3; i++){
        matrix[13+i] = ((ap_int<32>)nonce[4*i] << 24) | ((ap_int<32>)nonce[4*i + 1] << 16) | ((ap_int<32>)nonce[4*i + 2] << 8) | ((ap_int<32>)nonce[4*i + 3]);
    }
    //***************** Initializing State Matrix (End) *****************//

    //***************** Generating Cipher (Start) *****************//
    ap_int<8> cipher[len];
    ap_int<8> keystream[64];
    int num_blocks = len/64;
    int rem = len%64;

    for (int i = 0; i < num_blocks; i++){
        chacha20_block(matrix, block);
        for (int j = 0; j < 16; ++j) {
            keystream[4 * j] = block[i] >> 24;
            keystream[4 * j + 1] = block[i] >> 16;
            keystream[4 * j + 2] = block[i] >> 8;
            keystream[4 * j + 3] = block[i];
        }

        for (int j = 0; j < 64; j++){
            cipher[i*64 + j] = keystream[j] ^ plaintext[i*64 + j];
        }

        if (rem > 0){
            chacha20_block(matrix, block);
            for (int j = 0; j < 16; ++j) {
                keystream[4 * j] = block[i] >> 24;
                keystream[4 * j + 1] = block[i] >> 16;
                keystream[4 * j + 2] = block[i] >> 8;
                keystream[4 * j + 3] = block[i];
            }

            for (int j = 0; j < rem; j++){
                cipher[i*64 + j] = keystream[j] ^ plaintext[i*64 + j];
            }
        }
        matrix[12]++;
        
    }
    //***************** Generating Cipher (End) *****************//

    //***************** Writing Output to Stream (Start) *****************//
    for (int i = 0; i < len; i++){
        local_stream.data = cipher[i];
        i == len - 1 ? local_stream.last = 1 : local_stream.last = 0;
        output.write(local_stream);
    }
}