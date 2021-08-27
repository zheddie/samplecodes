#include <stdlib.h>
#include <time.h>

int rand_p(double p) {
    double r = (double)rand()/(double)RAND_MAX;

    if (r < p) return 1; //Returns with probability p
    else return -1;      //Returns with probability (1-p)
} 
