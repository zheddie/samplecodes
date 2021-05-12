#include <stdio.h>
#include <stdlib.h>

typedef union _zgILEpointer {
#if !(defined(_AIX) || defined(KERNEL))
#pragma pack(1,16,_ILEpointer)  /* Force sCC quadword alignment */
#endif
/* CAUTION: Some compilers only provide 64-bits for long double */
    long double   align;  /* Force xlc quadword alignment
           (with -qldbl128 -qalign=natural) */
#ifndef _AIX
    void    *openPtr; /* MI open pointer (tagged quadword) */
#endif
    struct {
  char    filler[8];
  void * addr; /* (PASE) memory address */
    } s;
} zgILEpointer
#if defined(__clang__) || defined(__GNUC__)
__attribute__((aligned(16)))
#endif
;
int main(int argc, char** argv)
{
	printf("sizeof(zgILEpointer)=%d\n",sizeof(zgILEpointer));
	char a ='X';
	printf("&a=%p\n",&a); 
	char b  __attribute__((aligned(8))) = 'X';
	printf("&b=%p\n",&b);
	zgILEpointer c ;
	printf("&zc=%p\n",&c);
	char a2 ='X';
	printf("&a2=%p\n",&a2);
	zgILEpointer c2 ;
	printf("&zc2=%p\n",&c2);	
#if defined(__clang__) 
	printf("__clang__ defined\n");
#endif
#if defined(__GNUC__) 
	printf("__GNU_C_VER__ defined\n");
#endif
#if defined(__xlc__)
	printf("__XLC_VER__ defined\n");
#endif
	return 0;
}

