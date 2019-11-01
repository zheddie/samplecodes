#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(void) {
  float f;
  double d;
  char *str = "1.1754943508222875E-38F";

  errno = 0;
  f = strtof(str, NULL);
  printf("strtof() errno = %d\n", errno);
  printf("result = %f\n", f);
  printf("result = %e\n", f);

  errno = 0;
  d = strtod(str, NULL);
  printf("strtod() errno = %d\n", errno);
  printf("result = %f\n", d);
  printf("result = %e\n", d);

  return 0;
}
