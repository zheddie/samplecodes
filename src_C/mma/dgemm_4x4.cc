#include <stdio.h>
#include <stdlib.h>
#define KM 4
#define KN 2
extern "C" void dgemm_kernel_4x2(double *, double *, double *, int, int, int, int);
void dgemm(double *A, double *B, double *C, int M, int N, int K)
{
    for (int i = 0; i < M; i += KM)
    {
        for (int j = 0; j < N; j += KN)
        {
            dgemm_kernel_4x2(A + i, B + j, C + j, K, M, N, N);
        }
        C += N * KM;
    }
}
void printF(const char *name, double *M, int m, int n)
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
    double A[M][K];
    double AT[K][M];
    double B[K][N];
    double C[M][N];
    double D[M][N];
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++){
            C[i][j] = 0;
            D[i][j] = 0;
        }
    int x = 1;
    for (int i = 0; i < M; i++)
        for (int j = 0; j < K; j++)
            A[i][j] = double(x++) * 7 / 15;
    for (int i = 0; i < K; i++)
        for (int j = 0; j < N; j++)
            B[i][j] = double(x++) * 3 / 17;
    for (int i = 0; i < M; i++)
        for (int j = 0; j < K; j++)
            AT[j][i] = A[i][j];
    dgemm((double *)AT, (double *)B, (double *)C, M, N, K);

    printF("A", (double *)A, M, K);
    printF("AT", (double *)AT, K, M);
    printF("B", (double *)B, K, N);
    printF("C", (double *)C, M, N);
    for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                for (int k = 0; k < K; k++)
                    D[i][j] += A[i][k] * B[k][j];
            }
        }    
    printF("D", (double *)D, M, N);
    return 0;
}
