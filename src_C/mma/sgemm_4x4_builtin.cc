#include <stdio.h>
#include <stdlib.h>
#define KM 4
#define KN 4
typedef vector unsigned char vec_t;
typedef __vector_quad acc_t;
void sgemm_kernel_4x4(float *a, float *b, float *c, int K, int lda, int ldb, int ldc)
{
    int i;
    vec_t vec_A, vec_B, vec_C[4];
    acc_t acc_0;
    __builtin_mma_xxsetaccz(&acc_0);
    for (i = 0; i < K; i++)
    {
        vec_A = *((vec_t *)(a + (i * lda)));
        vec_B = *((vec_t *)(b + (i * ldb)));
        __builtin_mma_xvf32gerpp(&acc_0, vec_A, vec_B);
    }
    __builtin_mma_disassemble_acc(vec_C, &acc_0);
    *((vec_t *)(c)) = vec_C[0];
    *((vec_t *)(c + ldc)) = vec_C[1];
    *((vec_t *)(c + (2 * ldc))) = vec_C[2];
    *((vec_t *)(c + (3 * ldc))) = vec_C[3];
}
void sgemm(float *A, float *B, float *C, int M, int N, int K)
{
    for (int i = 0; i < M; i += KM)
    {
        for (int j = 0; j < N; j += KN)
        {
            sgemm_kernel_4x4(A + i, B + j, C + j, K, M, N, N);
        }
        C += N * KM;
    }
}
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
int main(int argc, char **argv)
{
    if (argc < 4)
    {
        printf("Usage: %s <M> <N> <K> \n", argv[0]);
        return -1;
    }
    const int M = atoi(argv[1]);
    const int N = atoi(argv[2]);
    const int K = atoi(argv[3]);
    printf("Running: %s M=%s N=%s K=%s \n", argv[0], argv[1], argv[2], argv[3]);
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
    sgemm((float *)AT, (float *)B, (float *)C, M, N, K);

    printF("A", (float *)A, M, K);
    printF("B", (float *)B, K, N);
    printF("C", (float *)C, M, N);
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < K; k++)
                D[i][j] += A[i][k] * B[k][j];
        }
    }
    printF("D", (float *)D, M, N);
    return 0;
}
