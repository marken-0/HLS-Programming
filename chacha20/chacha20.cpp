#include "header.h"

#define leftShift(v, n) ( v<<n | v>>(32-n))

#define quarterRound(x, a, b, c, d) \
  x[a] += x[b]; x[d] = leftShift(x[d] ^ x[a], 16); \
  x[c] += x[d]; x[b] = leftShift(x[b] ^ x[c], 12); \
  x[a] += x[b]; x[d] = leftShift(x[d] ^ x[a], 8); \
  x[c] += x[d]; x[b] = leftShift(x[b] ^ x[c], 7);

void cc20_algo(hls::stream<axis_data> &input, hls::stream<axis_data> &output){
    //***************** Reading Input from Stream (Start) *****************//
    ap_int<32> key[8];
    ap_int<32> counter;
    ap_int<32> nonce[3];
    ap_int<32> plaintext[16];

    axis_data local_stream;

    for (int i = 0; i < 8; i++){
        local_stream = input.read();
        key[i] = local_stream.data;
    }

    local_stream = input.read();
    counter = local_stream.data;

    for (int i = 0; i < 3; i++){
        local_stream = input.read();
        nonce[i] = local_stream.data;
    }

    for (int i = 0; i < 16; i++){
        local_stream = input.read();
        plaintext[i] = local_stream.data;
    }
    //***************** Reading Input from Stream (End) *****************//

    ap_int<32> matrix[16], block[16], keystream[16];
    ap_int<32> constant[4] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};

    matrix[0] = constant[0]; matrix[1] = constant[1]; matrix[2] = constant[2]; matrix[3] = constant[3]; // Constant
    for (int i = 0; i < 8; i++) {
        matrix[4 + i] = key[i]; // Key
    }
    matrix[12] = counter; // Counter
    matrix[13] = nonce[0]; matrix[14] = nonce[1]; matrix[15] = nonce[2]; // Nonce

}