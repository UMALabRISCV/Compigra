#include <stdio.h>
#include <stdlib.h>

#define NI 12 
#define NJ 12  
#define NM 3
#define NK 3   

void conv2d(float input[NI][NJ],    
            float kernel[NM][NK],   
            float output[NI-NM+1][NJ-NK+1]) { 

    int input_height = NI;
    int input_width = NJ;
    int kernel_height = NM;
    int kernel_width = NK;

    int i, j, m, n;
    
    // Convolution operation
    for (i = 0; i < input_height - kernel_height + 1; i++) {
        for (j = 0; j < input_width - kernel_width + 1; j++) {
            output[i][j] = 0;
            for (m = 0; m < kernel_height; m++) {
                for (n = 0; n < kernel_width; n++) {
                    output[i][j] += input[i + m][j + n] * kernel[m][n];
                }
            }
        }
    }
}
