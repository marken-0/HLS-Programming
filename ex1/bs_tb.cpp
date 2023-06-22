#include "bs.h"

int binarySearch(int arr[], int size, int val, int* index) {
    int low = 0;
    int high = size - 1;
    
    while (low <= high) {
        int mid = low + (high - low) / 2;
        
        if (arr[mid] == val) {
            return mid;
        } else if (arr[mid] < val) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

int main(){
    int arr[dim] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int index;
    int val = 5;    

    hls::stream<axis_data> input, output;
    axis_data local_stream;

    for (int i = 0; i < dim; i++) {
        local_stream.data = arr[i];
        if (i == dim - 1) {
            local_stream.last = 1;
        } else {
            local_stream.last = 0;
        }
        input.write(local_stream);
    }
    
    local_stream.data = val;
    local_stream.last = 1;
    input.write(local_stream);

    bs(input, output);

    local_stream = output.read();
    index = local_stream.data;

    idx_bs_sw = binarySearch(arr, dim, val, &index);
    idx_bs_hw = index;
    if (idx_bs_sw == idx_bs_hw) {
        printf("No Error\n");
    } else {
        printf("Error\n");
    }

    return 0;
}
