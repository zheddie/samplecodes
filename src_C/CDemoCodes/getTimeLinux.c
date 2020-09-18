/*
 ============================================================================
 Name        : CDemos.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <stdio.h>
#include <sys/time.h>

int
main(void)
{
   /* get the time before the operation begins */
   printf("sizeof_int=%d,sizeof_long=%d,sizeof_long_long=%d\n",sizeof(int),sizeof(long),sizeof(long long));
   int sec = 1417070403;
   int nsec = 323989803;
   long zl1 = 0L;
   long long zl2 = 0LL;
   zl1 = (long)sec*1000+nsec/(1000*1000);
   zl2 = (long long) sec*1000+nsec/(1000*1000);
   printf("zl1=%ld\n",zl1);
   printf("zl2=%lld\n",zl2);
   printf("zl1=%lld\n",sec*1000+nsec/(1000*1000));
   return(0);
}
