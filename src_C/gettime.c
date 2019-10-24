#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
int main(){
	struct timeval t;
	long int i = 0;
	for(i=0;i<1000*1000*7;i++){
		gettimeofday(&t,NULL);
		//printf("Count = %ld\n",i);
		printf("total=%llu\n",t.tv_sec*1000*1000+t.tv_usec);
		gettimeofday(&t,NULL);
		printf("total=%llu\n",t.tv_sec*1000*1000+t.tv_usec);
	}
}
