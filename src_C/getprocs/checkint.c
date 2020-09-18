#include <stdio.h>
#include <string.h>
int main(){
	int x = -1242;
	unsigned char b[4];
	printf("sizeof(int):%lu\n",sizeof(int));
	memcpy(b,&x,sizeof(int));

	printf("bytes:%X,%X,%X,%X\n",b[0],b[1],b[2],b[3]);
}