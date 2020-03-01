#include <stdio.h>
void main(){
	char *s = "hello";
	char sp[10]="hello";
	char *spp[10];
	char a = 'a';
	enum XX{
		aa = 10,
		bb = 20,
		cc = 30
		};
	enum XX t = a;
	printf("sizeof: s=%d,sp=%d,a=%d,spp=%d,sizeof(enum)=%d,sizeof(enum XX a)=%d\n",sizeof(s),sizeof(sp),sizeof(a),sizeof(spp),sizeof(enum XX),sizeof(t));

}
