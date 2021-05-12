#include <stdio.h>
#pragma pack(1)
struct A{
int a1:4;
int a2:2;
} x;
char y;
int main(){
	char *px=(char *)&x;
	y='Z';
	x.a1=7;
	x.a2=2;
	printf("y:%p,x:%p,a1:,a2(%d)\n",&y,&x,__offsetof(struct A , a2));
	printf("[0]%x[1]%x[2]%x[3]%x[4]%x\n",px[0],px[1],px[2],px[3],px[4]);
}
