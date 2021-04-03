#include <stdio.h>

__global__ void add(int a, int b, int *c) {
    *c = a + b;
}

int main(void) {
    int c;
    int *dev_c;

    cudaMalloc( (void **) &dev_c, sizeof(int)); // fails here, returning 46

    add<<<1,1>>>(2,7,dev_c);
    cudaMemcpy(&c, dev_c, sizeof(int), cudaMemcpyDeviceToHost);
    printf("hello world, %d\n",c);
    cudaFree( dev_c);
    return 0;
}
