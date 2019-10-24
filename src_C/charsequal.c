#include <stdio.h>
#include <string.h>
main(){
	char * p = new char[20];
	char p1[10] = "Hello";
	char P2[] = "Hello";
	for(int i =0;i<20;i++){
		p[i] = 'a';
	}
	printf("zg.p(%p)=%s,p1(%p)=%s,p1len=%d,p2len=%d,p2size=%d\n",p,p,p1,p1,strlen(p1),strlen(p2),sizeof(p2));
	delete p,p1;
	printf("zg.delete done!\n");
}
