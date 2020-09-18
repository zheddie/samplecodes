#define _MULTI_THREADED
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
//#include "check.h"


int main(int argc, char **argv)
{
  int r=1;
  int izero = 0;

  double zero = 0.0;
  double average = 1;
  double zz = +0.0;
/*  r = r/izero;
  printf("r=%d\n",r);
  */
  average = average/zero;
  printf("average=%f\n",average);
  zz = zz/zero;
  printf("zz=%f\n",zz);
  return 0;
}
