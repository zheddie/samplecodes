#include <iostream>
#include <string>
#define VAL 0x0A0B0C0D

int main(){
	union s{
		int intv ;
		char c[4];
	} s1;
	//String xx ;
	//struct string xx;

	s1.intv = VAL;
	char * sp = NULL;
	sp = (char *)malloc(5);
	memset(sp,0,5);
	//printf("c[0]=%X,c[1]=%X,c[2]=%X,c[3]=%X\n",s1.c[0],s1.c[1],s1.c[2],s1.c[3]);
	sprintf(sp,"%X%X%X%X",s1.c[0],s1.c[1],s1.c[2],s1.c[3]);
	printf("int value=0x0A0B0C0D\n");
	printf("in memory=%s\n",sp);
	if(!strcmp(sp,"ABCD")){
		printf("This is big-endian system\n");
	}
	if(!strcmp(sp,"DCBA")){
		printf("This is little-endian system\n");
	}

}
