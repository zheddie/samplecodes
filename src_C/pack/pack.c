#include <stdio.h>
#pragma pack(2)
struct A{
	int a1:31;
	int a2:2;
};
struct B{
	char a1;
};
int main(){
        char * p=NULL;
	struct B x;
	struct A y;
	y.a1=2;
	y.a2=3;

	printf("a1(%p)=%d,sizeof(x(%p))=%d,sizeof(B)=%d\n",&(x.a1),x.a1,&x,sizeof(x),sizeof(struct B));
	printf("a1(%p)=%d,a2(%p)=%d,sizeof(y(%p))=%d,sizeof(A)=%d\n",&(y),y.a1,&y,y.a2,&y,sizeof(y),sizeof(struct A));
	p = (char *)&y;
	for(int i =0;i<sizeof(struct A);i++){
		printf("y[%d](%p)=%x,",i,p,*p);
		p++;
	}
	return(0);
}
