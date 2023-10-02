#include "cc20.h"

void chacha20_block(uint32_t state[16], uint32_t out[16]) {
	#pragma HLS PIPELINE
    for (int i = 0; i < 16; i++) {
		#pragma HLS PIPELINE
    	out[i] = state[i];
    }
    for (int i = 0; i < 10; i++) {
		#pragma HLS PIPELINE
        quarterRound(out, 0, 4, 8, 12); // @suppress("Suggested parenthesis around expression")
        quarterRound(out, 1, 5, 9, 13); // @suppress("Suggested parenthesis around expression")
        quarterRound(out, 2, 6, 10, 14); // @suppress("Suggested parenthesis around expression")
        quarterRound(out, 3, 7, 11, 15); // @suppress("Suggested parenthesis around expression")
        quarterRound(out, 0, 5, 10, 15); // @suppress("Suggested parenthesis around expression")
        quarterRound(out, 1, 6, 11, 12); // @suppress("Suggested parenthesis around expression")
        quarterRound(out, 2, 7, 8, 13); // @suppress("Suggested parenthesis around expression")
        quarterRound(out, 3, 4, 9, 14); // @suppress("Suggested parenthesis around expression")
    }
    for (int i = 0; i < 16; i++){
    	#pragma HLS PIPELINE
    	out[i] += state[i];
    }
}

void cc20_algo(hls::stream<axis_data> &input, hls::stream<axis_data> &output){
    //***************** Reading Input from Stream (Start) *****************//
	#pragma HLS INTERFACE axis register both port=output
	#pragma HLS INTERFACE axis register both port=input
	#pragma HLS INTERFACE ap_ctrl_none port=return

    uint8_t key[32];
    uint8_t plaintext[len];
    uint8_t counter_[4];
    uint32_t counter;
    uint8_t nonce[12];

    axis_data local_stream;


    for (int i = 0; i < 32; i++){
		#pragma HLS UNROLL
        local_stream = input.read();
        key[i] = local_stream.data;
    }

    for (int i = 0; i < 4; i++){
		#pragma HLS UNROLL
    	local_stream = input.read();
        counter_[i] = local_stream.data;
    }
    counter = counter_[0] << 24 | counter_[1] << 16 | counter_[2] << 8 | counter_[3];

    for (int i = 0; i < 12; i++){
		#pragma HLS UNROLL
        local_stream = input.read();
        nonce[i] = local_stream.data;
    }

    for (int i = 0; i < len; i++){
		#pragma HLS PIPELINE
        local_stream = input.read();
        plaintext[i] = local_stream.data;
    }

    //***************** Reading Input from Stream (End) *****************//

    //***************** Initializing State Matrix (Start) *****************//
    uint32_t matrix[16];
    uint32_t block[16];
    uint32_t constant[4] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574}; //Constants given in the algorithm

    matrix[0] = constant[0]; matrix[1] = constant[1]; matrix[2] = constant[2]; matrix[3] = constant[3]; // Constant
    for (int i = 0; i < 8; i++){
		#pragma HLS UNROLL
        matrix[4+i] = ((uint32_t)key[4*i + 3] << 24) | ((uint32_t)key[4*i + 2] << 16) | ((uint32_t)key[4*i + 1] << 8) | ((uint32_t)key[4*i]);
    }
    matrix[12] = counter; // Counter
    for (int i = 0; i < 3; i++){
		#pragma HLS UNROLL
        matrix[13+i] = ((uint32_t)nonce[4*i + 3] << 24) | ((uint32_t)nonce[4*i + 2] << 16) | ((uint32_t)nonce[4*i + 1] << 8) | ((uint32_t)nonce[4*i]);
    }
    //***************** Initializing State Matrix (End) *****************//

    //***************** Generating Cipher (Start) *****************//
    uint8_t cipher[len];
    uint8_t keystream[len];
    int num_blocks = len/64;
    int rem = len%64;

    for (int i = 0; i < num_blocks; i++){
		#pragma HLS PIPELINE
		chacha20_block(matrix, block);
		for (int j = 0; j < 16; ++j) {
			#pragma HLS UNROLL
            keystream[i*64 + 4*j + 3] = rightShift(block[j], 24);
            keystream[i*64 + 4*j + 2] = rightShift(block[j], 16);
            keystream[i*64 + 4*j + 1] = rightShift(block[j], 8);
            keystream[i*64 + 4*j] = block[j];
        }
        matrix[12]++;
    }

    if (rem != 0){
        chacha20_block(matrix, block);
        for (int j = 0; j < 16; ++j) {
			#pragma HLS UNROLL
            keystream[num_blocks*64 + 4*j + 3] = rightShift(block[j], 24);
            keystream[num_blocks*64 + 4*j + 2] = rightShift(block[j], 16);
            keystream[num_blocks*64 + 4*j + 1] = rightShift(block[j], 8);
            keystream[num_blocks*64 + 4*j] = block[j];
        }
    }

	for (int i = 0; i < len; i++){
		#pragma HLS PIPELINE
		cipher[i] = plaintext[i] ^ keystream[i];
	}
    //***************** Generating Cipher (End) *****************/

    //***************** Writing Output to Stream (Start) *****************//
    for (int i = 0; i < len; i++){
		#pragma HLS PIPELINE
    	local_stream.data = cipher[i];
        if (i == len-1){
        	local_stream.last = 1;
        }
        else {
        	local_stream.last = 0;
        }
        output.write(local_stream);
    }
    //***************** Writing Output to Stream (End) *****************//
    return;
}
