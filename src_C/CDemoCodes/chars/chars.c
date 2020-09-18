#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void func1(){
	
	char * ss[] = {"aaaa" "111" "222","bbbb","cccc"};
	char * pt[2];
	pt[0] = (char *) malloc(5);
	void * pp = (void *) malloc(7);
	pt[1] = (char *) malloc(5);
	printf("&pt[0]=%p,&pt[1]=%p,pt[0]=%p,pt[1]=%p\n",&pt[0],&pt[1],pt[0],pt[1]);
	strcpy(pt[0],"1111");
	strcpy(pt[1],"2222");
	char **p = ss; 
	//char **p = (void *)&pt[0]; 
	printf("pt=%llp,p=%llp\n",pt,p);
	printf("p[0]=%s\n",*p++);
	printf("pt=%llp,p=%llp\n",pt,p);
	printf("p[1]=%s\n",*p++);
	printf("pt=%llp,p=%llp\n",pt,p);
	//printf("p[2]=%s\n",*p++);

}
int main(){
	void (* f1)();
	int (*f2)();
	f1 = &func1;
	f2 = &main;
	printf("&func1=%llp,&main=%llp\n",f1,f2);
	(*f1)();
	printf("----------------------------------\n");
	f1();
	return(0);
}
