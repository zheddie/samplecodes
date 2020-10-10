/* @(#) $Id: //bas/753_COR/src/krn/o4port/o4ebcdic.c#1 $ SAP*/

/******************************************************************************************
copy as400_types to your build directory and call
xlclang  -D_ISOC99_SOURCE -D_AIX51 -D_LARGE_FILES -D__NO_XLC_BUILTIN_VA  -D_Packed=  -I.  -qfuncsect -U__unix -q64 -qtbtable=full -qalias=noansi -qutf -qdebug=BVSPACECONVERSATIVE -g -qlanglvl=stdc11   mytest.c 

******************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <as400_types.h>
typedef union _zgILEpointer2 {
	
    //long double   align;
    long double align;
    char ss[16];
} zgILEpointer2;
typedef union _zgILEpointer3 {
  struct {
	  char    filler[16-sizeof(uint64)];
	  address64_t addr; /* (PASE) memory address */
    } s;
} zgILEpointer3 __attribute__((aligned(16)));
// #ifdef __clang__
// #pragma align(16)
// #endif
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
int main(int argc, char** argv)
{
	printf("sizeof(zgILEpointer)=%d,sizeof(zgILEpointer2)=%d\n",sizeof(zgILEpointer),sizeof(zgILEpointer2));
	char a ='X';
	printf("&a=%p\n",&a); 
	char b  __attribute__((aligned(8))) = 'X';
	printf("&b=%p\n",&b);
	zgILEpointer3 c ;
	printf("&zc2=%p\n",&c);
	char d = 'A';
	printf("&d=%p\n",&d);
	zgILEpointer3 ff ;
	printf("&zff=%p\n",&ff);
	char e = 'B';
	printf("&e=%p\n",&e);
	char e2[7];
	char * pe = e2;
	printf("&e2=%p,&e2[0]=%p,e2[-2]=%X,e2[-1]=%X\n",&e2,&e2[0],*(pe-2),*(pe-1));
	zgILEpointer3 f ;
	printf("&zf=%p\n",&f);

	char g = 'B';
	printf("&e=%p\n",&e);
	// zgILEpointer2 *p ;
	
	// for(int i = 0;i<10;i++){
	// 	p = (zgILEpointer2 *)malloc(sizeof(zgILEpointer2 ));
	// 	printf("&zp=%p,&p=%p\n",p,&p);
	// }
	// char * pc = NULL;
	// for(int i = 0;i<10;i++){
	// 	pc = (char  *)malloc(7);
	// 	printf("&pc=%p,&pc=%p\n",pc,&pc);
	// }	
	return 0;
}

/* When I set a breakpoint at the return statement I can get the following:
(dbx) print &a
0x0ffffffffffffa80
(dbx) print &c
0x0ffffffffffffa98
(dbx) print &f
0x0ffffffffffffab0

so ILEpointer c is not 16-Byte alligned which should result in a problem
when used to exchange pointers with ILE
When I modify as400_types.h with my mentioned workaround all ILEpointers
start at 16-Byte alligned adresses.

*/
