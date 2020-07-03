#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
/******************************************
  Sample:
  A simple PASE for i Program called from
  ILE using QP2RunPase() and accepting
  one string parameter.
  The ILE sample program expects this to be
  located at /home/samplePASE. Compile on
  AIX, then ftp to IBM i.
  To ftp use the commands:
  > binary
  > site namefmt 1
  > put samplePASE /home/samplePASE
******************************************/

int main(int argc, char *argv[])
{
    /* Print out a greeting and the parameter passed in. Note argv[0] is the program
       name, so, argv[1] is the parameter */
     printf("Hello from PASE for i program %s. Parameter value is \"%s\",arg2:%s.pid:%d.\n", argv[0], argv[1],argv[2],getpid());

     return 0;
}