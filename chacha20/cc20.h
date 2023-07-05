#include <hls_stream.h>
#include <ap_int.h>
#include <stdio.h>
#include <stdint.h>

//#define len 114 // Test Vector 1
#define len 375 // Test Vector 2
//#define len 127 // Test Vector 3

#define leftShift(v, n) ( v<<n | v>>(32-n))
#define rightShift(v, n) ( v>>n | v<<(32-n))

#define quarterRound(x, a, b, c, d) \
	x[a] += x[b]; x[d] ^= x[a]; x[d] = leftShift(x[d], 16); \
  	x[c] += x[d]; x[b] ^= x[c]; x[b] = leftShift(x[b], 12); \
  	x[a] += x[b]; x[d] ^= x[a]; x[d] = leftShift(x[d], 8); \
  	x[c] += x[d]; x[b] ^= x[c]; x[b] = leftShift(x[b], 7);

struct axis_data {
	uint8_t data;
	ap_int<1> last;
};

void chacha20_block(uint32_t state[16], uint32_t out[16]);
void cc20_algo(hls::stream<axis_data> &input, hls::stream<axis_data> &output);
