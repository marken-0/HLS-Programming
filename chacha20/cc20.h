#include <hls_stream.h>
#include <ap_int.h>
#include <stdint.h>
#include <stdio.h>

#define len 64

#define leftShift(v, n) ( v<<n | v>>(32-n))

#define quarterRound(x, a, b, c, d) \
  x[a] += x[b]; x[d] = leftShift(x[d] ^ x[a], 16); \
  x[c] += x[d]; x[b] = leftShift(x[b] ^ x[c], 12); \
  x[a] += x[b]; x[d] = leftShift(x[d] ^ x[a], 8); \
  x[c] += x[d]; x[b] = leftShift(x[b] ^ x[c], 7);

struct axis_data {
    ap_int<8> data;
    ap_int<1> last;
};

void chacha20_block(ap_int<32> state[16], ap_int<32> out[16]);
void cc20_algo(hls::stream<axis_data> &input, hls::stream<axis_data> &output);
