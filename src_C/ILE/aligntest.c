#include <stdio.h>
#include "as400_types.h"
#include "as400_protos.h"

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
  char    filler[16-sizeof(uint64)];
  address64_t addr; /* (PASE) memory address */
    } s;
} zgILEpointer
#ifdef __clang__
__attribute__((aligned()))
#endif
;
int main(){
	
	zgILEpointer x;
	printf("sizeof(uint64)=%d==8\n",sizeof(uint64));
	printf("sizeof(zgILEpointer.s)=%d==16\n",sizeof(x.s));
	printf("sizeof(zgILEpointer.align)=%d==?\n",sizeof(x.align));
	printf("sizeof(zgILEpointer)=%d==16\n",sizeof(x));
	printf("&x=%p,&x.align=%p,&x.s.addr=%p\n",&x,&x.align,&x.s.addr);
#if !(defined(_AIX) || defined(KERNEL))
	printf("!(defined(_AIX) || defined(KERNEL))\n");
#else
	printf("FAILED.!(defined(_AIX) || defined(KERNEL))\n");
#endif
#ifdef __clang__
	printf("defined(__clang__)\n");
#else
	printf("FAILED:defined(__clang__)\n");
#endif   
#ifndef _AIX
	printf("!defined(_AIX)\n");
#else
	printf("FAILED.!defined(_AIX)\n");
#endif
	printf("done.\n");
    return(0);
}