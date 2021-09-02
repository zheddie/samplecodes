#include <stdio.h>
void f1(){
	printf("in f1\n");
}
void (*fp)();
int main(){
	fp = f1;
	(*fp)();
	fp =0x0;
	(*fp)();
	printf("in main\n");
}
