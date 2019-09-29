#include <omp.h>
int main() {
    int tna = omp_get_thread_num();
    int tca = omp_get_num_threads();
}
