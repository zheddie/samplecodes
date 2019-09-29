/*
 ============================================================================
 Name        : zgt1.c
 Author      : zg
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef ZG
#define PASENOFUNCRETURN(X)  return (X)
#else
#define PASENOFUNCRETURN(X) /*no op*/ 
#endif
#define DISPNAME(name) "hello/name"

int main(void) {
PASENOFUNCRETURN(-1);
	int i =8;
	int j = i+2;
	printf("!!!Hello World!!!j=%d\n",j); /* prints !!!Hello World!!! */
	printf(DISPNSME(name));
	printf(DISPNSME(age));
	return 0;
}
