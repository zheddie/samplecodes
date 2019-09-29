#include <cpuid.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/types.h>


int
main (void)
{
  int a, b, c, d;
  __cpuid (2 /* vendor string */, a, b, c, d);
  printf ("EAX: %x\nEBX: %x\nECX: %x\nEDX: %x\n", a, b, c, d);
  return 0;
}
