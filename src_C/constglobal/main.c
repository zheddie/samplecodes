#include <stdio.h>
#include "mod.h"
char * gpmodp= "MODP GLOBAL";
int main(){
	char * p = "hello const";
	char * pmodp= "Hello MODP";
	MODP=pmodp;
	printf("in main,MODP=%p,&MODP=%p\n",MODP,&MODP);
	printa(p);
	printb(p);
	printf("==================================================\n");
	MODP=gpmodp;
	printf("in main,MODP=%p,&MODP=%p\n",MODP,&MODP);
	printa(p);
	printb(p);
	return(0);
}
