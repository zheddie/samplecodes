#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
union myU{
struct {
	unsigned char a:7;
	unsigned char b:1;
	unsigned char e:2;
} d;
unsigned char c;
};
int main(){
	myU z;
	char y= 146;
	z.c=y;
	//memcpy(&x,&y,1);
	printf("zg.x.a=%d,x.b=%d,sizeof(myT)=%d\n",z.d.a,z.d.b,sizeof(myU));
	return(0);
}
