#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>
int (*pprint)(char *);
int main(){
	int rt = -1;
	void * handle = dlopen("S.so",RTLD_NOW);
	if(handle == NULL){
		printf("dlopen failed.errno=%d\n",errno);
		return(-1);
	}
	pprint = (int (*)(char *))dlsym(handle,"print");
	if(pprint == NULL){
		printf("print failed to dlsym.errno:%d\n",errno);
		return(-1);
	}
	rt = (*pprint)("Eddie");
	printf("In main:rt=%d\n",rt);
	return(0);
}
