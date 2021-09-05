#include <stdio.h>
#include <stdlib.h>
#define KM 4
#define KN 3
typedef vector unsigned char vec_t;
typedef __vector_quad acc_t;
void printF(const char *name, float *M, int m, int n)
{
    printf("\n**** Matrix %s****\n", name);
    for (int i = 0; i < m; i++)
    {
        printf("| ");
        for (int j = 0; j < n; j++)
            printf("%-25.4f", *(M++));
        printf(" |\n");
    }
    printf("************************\n");
}
void sgemm_kernel_4x3(float *a, float *b, float *c, int K, int lda, int ldb, int ldc)
{
    int i;
    vec_t vec_A, vec_B, vec_C[4];
    acc_t acc_0;
    __builtin_mma_xxsetaccz(&acc_0);
    for (i = 0; i < K; i++)
    {
        vec_A = *((vec_t *)(a + (i * lda)));
        vec_B = *((vec_t *)(b + (i * ldb)));
        //__builtin_mma_xvf32gerpp(&acc_0, vec_A, vec_B);
        __builtin_mma_pmxvf32gerpp(&acc_0, vec_A, vec_B,0xF,0xE);
    }
    __builtin_mma_disassemble_acc(vec_C, &acc_0);
    // *((vec_t *)(c)) = vec_C[0];
    // *((vec_t *)(c + ldc)) = vec_C[1];
    // *((vec_t *)(c + (2 * ldc))) = vec_C[2];
    // *((vec_t *)(c + (3 * ldc))) = vec_C[3];
    for(int j = 0;j<KN*4;j++){
        *(((unsigned char *)c+j)) =vec_C[0][j];
        *(((unsigned char *)c+j + ldc*4)) =  vec_C[1][j];
        *(((unsigned char *)c+j + (2 * ldc)*4)) = vec_C[2][j];
        *(((unsigned char *)c+j + (3 * ldc)*4)) = vec_C[3][j];
    }
}
void sgemm(float *A, float *B, float *C, int M, int N, int K)
{
    float * pC = C;
    for (int i = 0; i < M; i += KM)
    {
        for (int j = 0; j < N; j += KN)
        {
            sgemm_kernel_4x3(A + i, B + j, C + j, K, M, N, N);
        }
        //To trace it.
        // for(int k =0;k<KM;k++){
        //     for (int l=0;l<KN;l++){
        //         printf("%f ",C[k][l]);
        //     }
        //     printf("\n");
        // }
        printF("pC", (float *)pC, M, N);
        getchar();
        C += N * KM;
    }
}

int main(int argc, char **argv)
{

    const int M = 8, N = 9, K = 4;
    float A[M][K];
    float AT[K][M];
    float B[K][N];
    float C[M][N];
    float D[M][N];
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
        {
            C[i][j] = 0;
            D[i][j] = 0;
        }
    int x = 1;
    for (int i = 0; i < M; i++)
        for (int j = 0; j < K; j++)
            A[i][j] = float(x++) * 7 / 15;
    for (int i = 0; i < K; i++)
        for (int j = 0; j < N; j++)
            B[i][j] = float(x++) * 3 / 17;
    for (int i = 0; i < M; i++)
        for (int j = 0; j < K; j++)
            AT[j][i] = A[i][j];
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < K; k++)
                D[i][j] += A[i][k] * B[k][j];
        }
    }
    printF("D", (float *)D, M, N);
    sgemm((float *)AT, (float *)B, (float *)C, M, N, K);

    printF("A", (float *)A, M, K);
    printF("B", (float *)B, K, N);
    printF("C", (float *)C, M, N);

    return 0;
}
