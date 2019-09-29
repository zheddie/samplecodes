#include <iostream>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

int main()
{
  int a, b;
  unsigned cpu,node;

  for (a = 0; a < 5; a++)
  {
    __asm__("cpuid;"
            :"=a"(b)                 // EAX into b (output)
            :"0"(a)                  // a into EAX (input)
            :"%ebx","%ecx","%edx");  // clobbered registers

    std::cout << "The code " << a << " gives " << b << std::endl;
  }
  int rt = syscall(SYS_getcpu,&cpu,&node,NULL);
  std::cout<<"rt="<<rt<<",cpu="<<cpu<<",node="<<node<<std::endl;


  return 0;
}
