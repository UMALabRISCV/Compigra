#include <stdio.h>

#define NI 60
#define NJ 60
#define NK 60

void mmul_base(int inputX[NI][NK], int inputY[NK][NJ], int output[NI][NJ]) {
  int i,j,k;
  int sum;

  for(int i = 0; i < NI; i ++) {
        for(int j = 0; j < NJ; j ++) {
            int sum = 0;
            for(int k = 0; k < NK; k++) {
                sum += inputX[i][k] * inputY[k][j];
            }
            output[i][j] = sum;
        }
    }
}