#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define LL long long 
void fun1(LL s){
	printf("s=%lld\n",s);
}

int main(){
	int x = 1048576, y=1048576;
	LL l = 0L;
	printf("sizeof(int)=%lu,sizeof(LL)=%lu\n",sizeof(int),sizeof(LL));
	printf("fun1(1048576*1048576):");
	fun1(1048576*1048576);
	printf("fun1((LL)1048576*1048576):");
	fun1((LL )1048576*1048576);
	printf("fun1((LL) (1048576*1048576)):");
	fun1((LL )(1048576*1048576));
	
	printf("fun1(x*y):");
	fun1(x*y);
	printf("fun1((LL)x*y):");
	fun1((LL) x*y);
	printf("fun1((LL)(x*y)):");
	fun1((LL) (x*y));
	
	l = x*y;
	printf("fun1(l=x*y):");
	fun1(l);
	l = (LL) x*y;
	printf("fun1(l=(LL)x*y):");
	fun1(l);
	l = (LL) (x*y);
	printf("fun1(l=(LL)(x*y)):");
	fun1(l);

	printf("main.exit\n");
}
