#include "bs.h"

void bs(hls::stream<axis_data> &input, hls::stream<axis_data> &output){
    int arr[dim];
    int val;
    int index;

    axis_data local_stream;

    for (int i = 0; i < dim; i++) {
        local_stream = input.read();
        arr[i] = local_stream.data;
    }

    local_stream = input.read();
    val = local_stream.data;

    low = 0;
    high = dim - 1;
    for (;low <= high;) {
        int mid = low + (high - low) / 2;
        if arr[mid] == val {
            index = mid;
            break;
        } else if arr[mid] < val {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    // index = -1;

    local_stream.data = index;
    local_stream.last = 1;
    output.write(local_stream);
    return;
}