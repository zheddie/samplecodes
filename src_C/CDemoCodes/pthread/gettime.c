#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
int main(){
	struct timeval t;
	gettimeofday(&t,NULL);
	printf("sec=%ld,usec=%ld\n",t.tv_sec,t.tv_usec);
	sleep(1);
	gettimeofday(&t,NULL);
	printf("sec=%ld,usec=%ld\n",t.tv_sec,t.tv_usec);



}
