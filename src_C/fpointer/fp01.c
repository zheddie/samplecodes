#include <stdio.h>
int f1(int);
int (*fp1)(int);
int main(){
	int b = f1(8);
	int c = f1(9);
	fp1 = &f1;
	int d = fp1(10);
	int e = (*fp1)(11);
	printf("b=%d,c=%d,d=%d,d=%d\n",b,c,d,e);
}
int f1(int a){
	return a * a;
}
