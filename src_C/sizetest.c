#include <stdio.h>
struct S{
	int x;
	char t[5];
	char *p;
};
struct S1{
	int x;
	char t[5];
	char *p;
	enum EN{
		YES,
		NO,
		POSSIBLE
	};
	enum EN en_s;
};
int main(){

	printf("sizeof(int)=%d,sizeof(S)=%d\n",sizeof(int),sizeof(struct S));
	printf("sizeof(S)=%d\n",sizeof(struct S1));
}
