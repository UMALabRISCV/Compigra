#include <stdio.h>

#define NL 4
#define NI 60
#define NJ 60
#define NK 60

void mmul_batch(int inputX[NL][NI][NK], 
                int inputY[NL][NK][NJ], 
                int output[NL][NI][NJ]) {
  int b, i,j,k;
  int sum;

  for(int b = 0; b < NL; b ++)
    for( i = 0; i < NI; i ++) {
      for( j = 0; j < NJ; j ++) {
          int sum = 0;
          for( k = 0; k < NK; k++) {
            sum += inputX[b][i][k] * inputY[b][k][j];
          }
          output[b][i][j] = sum;
      }
    }
}