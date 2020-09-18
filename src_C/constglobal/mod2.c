#include <stdio.h>
#include "mod.h"
void printb(char * s){
	printf("printb,s=%s\n",s);
	printf("printb,MODP=%s\n",MODP);
	printf("in printb,MODP=%p,&MODP=%p\n",MODP,&MODP);
}
