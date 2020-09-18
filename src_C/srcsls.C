#include <cstdio>
#include <cstring>
#include <cstdlib>
#define BS 16
extern "C" void * func2__FPc(char *);
extern "C" int * func3__Fv();
void func1(char * p){
	printf("string(%d:%p):%s\n",sizeof(p),p,p);
}
int main(){
	union{
		char * up;
		void * vup;
		void * (*fup)(char *);
		void (*ff)(char *);
		char cp[BS];
	} u;

	int rt = 0;
	void (*fp)(char *)  = func1; 
	void *(*fp2)(char *) = func2__FPc;
	int * (*fp3)() = func3__Fv;
	printf("rt(%d)=%d,fp(%d)=%p\n",sizeof(int),rt,sizeof(fp),fp);	
	u.ff = fp;
	printf("fp(X)=");
	for(int i =0;i<BS;i++){
		printf("%02X",u.cp[i]);
	}
	printf("\n");
	u.fup = fp2;
	printf("fp2(X)=");
	for(int i =0;i<BS;i++){
		printf("%02X",u.cp[i]);
	}
	printf("\n");
	char *s=NULL;
	s = (char *)malloc(100);
	memset(s,0,100);
	strcpy(s, "HelloIBMi");
	u.up = s;
	printf("string(X)=");
	for(int i =0;i<BS;i++){
		printf("%02X",u.cp[i]);
	}
	printf("\n");

	fp((char *)s);
	void * ppp = fp2(s);
	u.vup = ppp;
	printf("ppp(X)=");
	for(int i =0;i<BS;i++){
		printf("%02X",u.cp[i]);
	}
	printf("\n");
	
	printf("SLS(%d:%p):%s\n",sizeof(ppp),ppp,(char *)ppp);
	int *ip  = fp3();
	printf("zg.in main.ip = %d\n",*ip);
	printf("still running..\n");
	return(0);
}
