#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]){
typedef struct AA{
	int a;
	char b;
	} AA;
typedef struct BB{
	char b;
	long c;
} BB;
void * p = (void *) malloc(10);
void * p0 = p+1;
void * p1 = (AA *) p +1;
void * p2 = (BB *) p + 1;
printf("zg.p=%llp,xp0=%llp,p1=%llp,p2=%llp,sizeofAA=%d,sizeofBB=%d,sizeofint=%d,sizeofvoid*=%d\n",p,p0,p1,p2,sizeof(AA),sizeof(BB),sizeof(int),sizeof(void*));

}
