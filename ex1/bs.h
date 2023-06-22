#include <hls_stream.h>
#include <ap_int.h>
#include <stdio.h>

#define dim 10

struct axis_data {
    int data;
    ap_uint<1> last;
};

void bs(hls::stream<axis_data> &input, hls::stream<axis_data> &output);