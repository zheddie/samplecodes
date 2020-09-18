#include <float.h>
#include <stdio.h>
 
static float my_min(void) { return FLT_MIN; }
main() {
  float fl = my_min();
  printf("fl=%f\n");
  return 0;
}
