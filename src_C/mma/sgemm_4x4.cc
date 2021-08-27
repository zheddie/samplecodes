#include <stdio.h>
#include <stdlib.h>
#define KM 4
#define KN 4
extern "C" void sgemm_kernel_4x4(float *, float *, float *, int, int, int, int);
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
        for (int j = 0; j < N; j++){
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
