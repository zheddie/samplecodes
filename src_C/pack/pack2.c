#include <stdio.h>
struct A1{
	short a1;
	char s1[5];
	short a2;
};
#pragma pack(1)
struct A2{
	short a1;
	char s1[5];
	short a2;
};
#pragma pack(pop)
struct A3{
#pragma pack(1)
	short a1;
	char s1[5];
	short a2;
#pragma pack(pop)	
};
#pragma pack(16)
struct A4{
	short a1;
	char s1[5];
	short a2;
};
#pragma pack(pop)

#pragma pack(16)
int main(){
	
	char c1;
	struct A1 v1;
	char c2;
	struct A2 v2;
	char c3;
	struct A3 v3;
	char c4;
	struct A4 v4;

	printf("c1(%p),v1(%p).s1(%d).a2(%d)\n",&c1,&v1,__offsetof(struct A1,s1),__offsetof(struct A1,a2));
	printf("c2(%p),v2(%p).s1(%d).a2(%d)\n",&c2,&v2,__offsetof(struct A2,s1),__offsetof(struct A2,a2));
	printf("c3(%p),v3(%p).s1(%d).a2(%d)\n",&c3,&v3,__offsetof(struct A3,s1),__offsetof(struct A3,a2));
	printf("c4(%p),v4(%p).s1(%d).a2(%d)\n",&c4,&v4,__offsetof(struct A4,s1),__offsetof(struct A4,a2));

	return(0);
}
