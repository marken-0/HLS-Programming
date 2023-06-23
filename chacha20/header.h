#include <hls_stream.h>
#include <ap_int.h>
#include <stdint.h>
#include <stdio.h>

#define len 64

struct axis_data {
    ap_int<8> data;
    ap_int<1> last;
};
