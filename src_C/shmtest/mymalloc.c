#include <stdio.h>
#include <unistd.h>
#include <errno.h>
int main(int argc , char * argv[]){
	int size = 2;
	unsigned long S1G = 0x40000000;
	unsigned long S1M = 0x100000;
#ifdef DATA64
	unsigned long ONES = S1G;
#else
	unsigned long ONES = S1M;
#endif
	void * p = NULL;
	if(argc > 1){
		size = atoi(argv[1]);
	}
#ifndef DATA64
	if(size < 2048){
#endif
		p = (void *)malloc(ONES*size);
		if( NULL != p){
			for (int x = 0;x<size;x++){
				memset(p,0,ONES);
				printf("+%d,p=%p\n",x,p);
				p = (void *)((char *)p+ONES);
			}
		}else{
			printf("malloc(1) failed.errno:%d\n",errno);
		}
#ifndef DATA64
	}else{
		p = (void *)malloc(ONES*2047);

		if( NULL != p){
			for (int x = 0;x<2047;x++){
				memset(p,0,ONES);
				printf("+%d,p=%p\n",x,p);
				p = (void *)((char *)p+ONES);
			}
		}else{
			printf("malloc(2) failed.errno:%d\n",errno);
		}
		for(int y = 0;y<size-2047;y++){
			p = (void *)malloc(ONES);
			if( NULL != p){
				memset(p,0,ONES);
				printf("y:%dp=%p\n",y,p);
			}else{
				printf("malloc(3) failed.errno:%d\n",errno);
				exit(-1);
			}
		}
	}
#endif
	printf("END\n");
	return(0);
}