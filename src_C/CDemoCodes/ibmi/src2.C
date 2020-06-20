#include <cstdio>
#include <cstring>
#include <cstdlib>
void * func2(char * p){
	union{
		char * up;
		char cp[16];
	} u;
	u.up = p;
	printf("string(%d:%p):%s\n",sizeof(p),p,p);
	printf("p(X)=");
	for(int i =0;i<16;i++){
		printf("%02X",u.cp[i]);
	}
	printf("\n");
	char * s = NULL;
	s = (char *)malloc(100);
	memset(s,0,100);
	strcpy(s, "mallocedinfunc2");
	printf("s(%d:%p)=%s\n",sizeof(s),s,s);
	u.up = s;
	printf("s(X)=");
	for(int i =0;i<16;i++){
		printf("%02X",u.cp[i]);
	}
	printf("\n");
	return((void *)s);
}
