// #include <hls_stream.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define leftShift(v, n) ( v<<n | v>>(32-n))
#define rightShift(v, n) ( v>>n | v<<(32-n))

#define quarterRound(x, a, b, c, d) \
	x[a] += x[b]; x[d] ^= x[a]; x[d] = leftShift(x[d], 16); \
  	x[c] += x[d]; x[b] ^= x[c]; x[b] = leftShift(x[b], 12); \
  	x[a] += x[b]; x[d] ^= x[a]; x[d] = leftShift(x[d], 8); \
  	x[c] += x[d]; x[b] ^= x[c]; x[b] = leftShift(x[b], 7);


void textToHexArray(const char* text, uint8_t hexArray[], size_t length) {
    for (size_t i = 0; i < length; ++i) { hexArray[i] = (uint8_t)text[i];}
}

void chacha20_block(uint32_t state[16], uint32_t out[16]) {
    for (int i = 0; i < 16; ++i) out[i] = state[i];
    for (int i = 0; i < 10; i ++) {
        quarterRound(out, 0, 4, 8, 12) // @suppress("Suggested parenthesis around expression")
        quarterRound(out, 1, 5, 9, 13) // @suppress("Suggested parenthesis around expression")
        quarterRound(out, 2, 6, 10, 14) // @suppress("Suggested parenthesis around expression")
        quarterRound(out, 3, 7, 11, 15) // @suppress("Suggested parenthesis around expression")
        quarterRound(out, 0, 5, 10, 15) // @suppress("Suggested parenthesis around expression")
        quarterRound(out, 1, 6, 11, 12) // @suppress("Suggested parenthesis around expression")
        quarterRound(out, 2, 7, 8, 13) // @suppress("Suggested parenthesis around expression")
        quarterRound(out, 3, 4, 9, 14) // @suppress("Suggested parenthesis around expression")
    }
    for (int i = 0; i < 16; ++i) out[i] += state[i];
}
// void cc20_algo(hls::stream<axis_data> &input, hls::stream<axis_data> &output);
