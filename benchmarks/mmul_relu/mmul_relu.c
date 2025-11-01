#include <stdio.h>

#define NI 25
#define NK 25
#define NJ 25

void mmul_relu(int inputX[NI][NK], int inputY[NK][NJ], int output[NI][NJ]) {
  int i,j,k;
  int sum;

  for(int i = 0; i < NI; i ++) {
        for(int j = 0; j < NJ; j ++) {
            int sum = 0;
            for(int k = 0; k < NK; k++) {
                sum += inputX[i][k] * inputY[k][j];
            }
            
            if (sum < 0) {
                sum = 0;
            }
            output[i][j] = sum;
        }
    }

}
