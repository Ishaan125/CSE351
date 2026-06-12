/*
 * CSE 351 Lab 4 (Caches and Cache-Friendly Code)
 * Part 2 - Optimizing Matrix Transpose
 *
 * Name(Ishaan Srivastava):  
 * NetID(ishaan03): 
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[M][N], int B[N][M]);
 * and compute B = A^T.
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1 KiB direct mapped cache with a block size of 32 bytes.
 */

#include <stdio.h>
#include "support/cachelab.h"

int is_transpose(int M, int N, int A[M][N], int B[N][M]);


/*
 * transpose_submit - This is the transpose function that you will be graded
 *     on. Do not change the description string "Transpose submission", as the
 *     driver searches for that string to identify the transpose function to be
 *     graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[M][N], int B[N][M]) {
    int i, j, row;
    int v0, v1, v2, v3, v4, v5, v6, v7;

    if (M == 32 && N == 32) {
        for (i = 0; i < 32; i += 8) {
            for (j = 0; j < 32; j += 8) {
                for (row = i; row < i + 8; row++) {
                    v0 = A[row][j];
                    v1 = A[row][j+1];
                    v2 = A[row][j+2];
                    v3 = A[row][j+3];
                    v4 = A[row][j+4];
                    v5 = A[row][j+5];
                    v6 = A[row][j+6];
                    v7 = A[row][j+7];
                    B[j][row]   = v0;
                    B[j+1][row] = v1;
                    B[j+2][row] = v2;
                    B[j+3][row] = v3;
                    B[j+4][row] = v4;
                    B[j+5][row] = v5;
                    B[j+6][row] = v6;
                    B[j+7][row] = v7;
                }
            }
        }
    } 
    else if (M == 64 && N == 64) {
        for (i = 0; i < 64; i += 4) {
            for (j = 0; j < 64; j += 4) {
                for (row = i; row < i + 4; row++) {
                    v0 = A[row][j];
                    v1 = A[row][j+1];
                    v2 = A[row][j+2];
                    v3 = A[row][j+3];
                    B[j][row]   = v0;
                    B[j+1][row] = v1;
                    B[j+2][row] = v2;
                    B[j+3][row] = v3;
                }
            }
        }
    }
}


// You can define additional transpose functions below. We've defined a simple
// one below to help you get started.

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[M][N], int B[N][M]) {
    int i, j, tmp;

    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

}


/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions() {
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);

}


/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[M][N], int B[N][M]) {
    int i, j;

    for (i = 0; i < M; i++) {
        for (j = 0; j < N; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}
