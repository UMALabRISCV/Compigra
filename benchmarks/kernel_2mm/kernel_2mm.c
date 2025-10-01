/**
 * This version is stamped on May 10, 2016
 *
 * Contact:
 *   Louis-Noel Pouchet <pouchet.ohio-state.edu>
 *   Tomofumi Yuki <tomofumi.yuki.fr>
 *
 * Web address: http://polybench.sourceforge.net
 */
/* 2mm.c: this file is part of PolyBench/C */

#include <stdio.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NI 8
#define NJ 8
#define NK 8
#define NL 8
#define ALPHA 3
#define BETA 6

void kernel_2mm(int alpha_ptr[1],
                int beta_ptr[1],
                int tmp[NI][NJ],
                int A[NI][NK],
                int B[NK][NJ],
                int C[NJ][NL],
                int D[NI][NL])
{
    int i, j, k;
    int ni = NI;
    int nj = NJ;
    int nk = NK;
    int nl = NL;

    /* D := alpha*A*B*C + beta*D */
    for (i = 0; i < ni; i++)
        for (j = 0; j < nj; j++)
        {
            tmp[i][j] = 0;
            for (k = 0; k < nk; ++k)
                tmp[i][j] += ALPHA * A[i][k] * B[k][j];
        }
    for (i = 0; i < ni; i++)
        for (j = 0; j < nl; j++)
        {
            D[i][j] *= BETA;
            for (k = 0; k < nj; ++k)
                D[i][j] += tmp[i][k] * C[k][j];
        }
}
