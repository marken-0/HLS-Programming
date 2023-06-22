#include <stdio.h>
#include <hls_stream.h>
#include <ap_int.h>

#define dim 20

struct axis_data {
    int data;
    ap_uint<1> last;
};
