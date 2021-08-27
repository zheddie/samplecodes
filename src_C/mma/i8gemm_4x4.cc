#include <stdio.h>
#include <stdlib.h>
#define KM 4
#define KN 4
#define Q 4
extern "C" void i8gemm_mma_4x4(int8_t *, uint8_t *, int32_t *, int, int, int, int);
void i8gemm(int8_t *A, uint8_t *B, int32_t *C, int M, int N, int K)
{
    int8_t *At = (int8_t *)malloc(M * K);   //transform A[M][K] --> At[K/Q][M*Q]
    uint8_t *Bt = (uint8_t *)malloc(N * K); //transform B[K][N] --> At[K/Q][N*Q]
    for (int i = 0, x = 0; i < K; i += Q)
    {
        for (int j = 0; j < M; j++)
        {
            for (int l = 0; l < Q; l++)
                At[x++] = *(A + (j * K) + l);
        }
        A = A + Q;
    }
    for (int i = 0, x = 0; i < K; i += Q)
    {
        for (int j = 0; j < N; j++)
        {
            for (int l = 0; l < Q; l++)
                Bt[x++] = *(B + (l * N) + j);
        }
        B += Q * N;
    }
    for (int i = 0; i < M; i += KM)
    {
        for (int j = 0; j < N; j += KN)
        {
            i8gemm_mma_4x4(At + (i * Q), Bt + (j * Q), C + j, K / Q, M, N, N);
        }
        C += N * KM;
    }
}
void printI(const char *name, int32_t *M, int m, int n)
{
    printf("\n**** Matrix %s****\n", name);
    for (int i = 0; i < m; i++)
    {
        printf("| ");
        for (int j = 0; j < n; j++)
            printf("%-25d", *(M++));
        printf(" |\n");
    }
    printf("************************\n");
}
void printIA(const char *name, int8_t *M, int m, int n)
{
    printf("\n**** Matrix %s****\n", name);
    for (int i = 0; i < m; i++)
    {
        printf("| ");
        for (int j = 0; j < n; j++)
            printf("%-25d", *(M++));
        printf(" |\n");
    }
    printf("************************\n");
}
void printIB(const char *name, uint8_t *M, int m, int n)
{
    printf("\n**** Matrix %s****\n", name);
    for (int i = 0; i < m; i++)
    {
        printf("| ");
        for (int j = 0; j < n; j++)
            printf("%-25d", *(M++));
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
    int8_t A[M][K];
    uint8_t B[K][N];
    int32_t C[M][N];
    int32_t D[M][N];
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            {
                C[i][j] = 0;
                D[i][j] = 0;
            }    
    int x = 1;
    for (int i = 0; i < M; i++)
        for (int j = 0; j < K; j++)
            A[i][j] = (x++) % 128;
    for (int i = 0; i < K; i++)
        for (int j = 0; j < N; j++)
            B[i][j] = (x++) % 256;
    i8gemm((int8_t *)A, (uint8_t *)B, (int32_t *)C, M, N, K);
    printIA("A", (int8_t *)A, M, K);
    printIB("B", (uint8_t *)B, K,N);
    printI("C", (int32_t *)C, M, N);
    for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                for (int k = 0; k < K; k++)
                    D[i][j] += A[i][k] * B[k][j];
            }
        }
    printI("D", (int32_t *)D, M, N);
    return 0;
}
