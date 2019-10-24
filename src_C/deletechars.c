#include <stdio.h>
main(){
	char * p = new char[20];
	for(int i =0;i<20;i++){
		p[i] = 'a';
	}
	printf("zg.p=%s\n",p);
	delete p;
	printf("zg.delete done!\n");
}
