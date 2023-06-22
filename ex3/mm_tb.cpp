#include "mm.h"

void mm_sw(int mat1[dim][dim], int mat2[dim][dim], int res[dim][dim]){
    for(int i=0; i < dim; i++){
        for(int j=0; j < dim; j++){
            res[i][j] = 0;
            for(int k=0; k < dim; k++){
                res[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }
}

int main(){
    int mat1[dim][dim], mat2[dim][dim], res_sw[dim][dim], res_hw[dim][dim];

    for (int i=0; i < dim; i++){
        for (int j=0; j < dim; j++){
            mat1[i][j] = i;
            mat2[i][j] = j;
        }
    }

    hls::stream<axis_data> input, output;
    axis_data local_stream;

    for (int i=0; i < dim; i++){
        for (int j=0; j < dim; j++){
            local_stream.data = mat1[i][j];
            if (i == dim-1 && j == dim-1){
                local_stream.last = 1;
            } else {
                local_stream.last = 0;
            }
            input.write(local_stream);
        }
    }

    for (int i=0; i < dim; i++){
        for (int j=0; j < dim; j++){
            local_stream.data = mat2[i][j];
            if (i == dim-1 && j == dim-1){
                local_stream.last = 1;
            } else {
                local_stream.last = 0;
            }
            input.write(local_stream);
        }
    }

    mm_hw(input, output);
    for (int i=0; i < dim; i++){
        for (int j=0; j < dim; j++){
            res_hw[i][j] = output.read().data;
        }
    }

    mm_sw(mat1, mat2, res_sw);

    for (int i=0; i < dim; i++){
        for (int j=0; j < dim; j++){
            if (res_hw[i][j] != res_sw[i][j]){
                printf("Error\n");
            }
        }
    }
    printf("No Error\n");

    return 0;
}