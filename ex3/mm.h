#include <hls_stream.h>
#include <ap_int.h>
#include <stdio.h>

#define dim 4

struct axis_data {
    int data;
    ap_uint<1> last;
};