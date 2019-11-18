#include <stdio.h>
#include "mod.h"
void printa(char * s){
	printf("printa,s=%s\n",s);
	printf("printa,MODP=%s\n",MODP);
	printf("in printa,MODP=%p,&MODP=%p\n",MODP,&MODP);
}
