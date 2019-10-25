#include <stdbool.h>
#define MAX 100
#define MIN 10

bool verify(int var){
    return ((var > MAX ? false : var) > MIN) ? true : false;
}
