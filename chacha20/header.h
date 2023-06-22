#include <hls_stream.h>
#include <ap_int.h>
#include <stdint.h>
#include <stdio.h>

#define dim 10

struct axis_data {
    int data;
    ap_int<1> last;
};
