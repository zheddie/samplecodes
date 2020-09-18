#include <stdio.h>
#include <stdlib.h>
int main(){
	int hl = 0;
	char *p=(char *)malloc(20);
	printf("zg.try fscanf!\n");
	fscanf(stdin,"str:(%s),hl:(%d)",p,&hl);
	printf("zg.fscanf,str:%s,hl=%d!\n",p,hl);
	//printf("zg.fscanf,hl=%d!\n",hl);
}