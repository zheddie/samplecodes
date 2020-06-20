#include <stdio.h>
typedef struct ss{
   int a ;
   int b;
   }TT;
main(){
	struct ss c;
	ss d;
	TT e;
	e.b = 3;
	d.a = 5;
	printf("zg.d.a=%d,c.a=%d,e.b = %d\n",d.a,c.a,e.b);

}
