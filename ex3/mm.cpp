#include "mm.h"

void mm_hw(hls::stream<axis_data> &input, hls::stream<axis_data> &output){
    int mat1[dim][dim], int mat2[dim][dim], int res[dim][dim];
    for (int i=0; i < dim; i++){
        for (int j=0; j < dim; j++){
            mat1[i][j] = input.read().data;
        }
    }

    for (int i=0; i < dim; i++){
        for (int j=0; j < dim; j++){
            mat2[i][j] = input.read().data;
        }
    }
    
    for (int i=0; i < dim; i++){
        for (int j=0; j < dim; j++){
            res[i][j] = 0;
            for (int k=0; k < dim; k++){
                res[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }

    for (int i=0; i < dim; i++){
        for (int j=0; j < dim; j++){
            axis_data local_stream;
            local_stream.data = res[i][j];
            if (i == dim-1 && j == dim-1){
                local_stream.last = 1;
            } else {
                local_stream.last = 0;
            }
            output.write(local_stream);
        }
    }

    return;
}