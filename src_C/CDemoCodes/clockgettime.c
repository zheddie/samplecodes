#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
long long int gettime(){
	struct timespec now;
	memset(&now,0,sizeof(struct timespec));
	long long int llTime = 0 ;
	int rc = clock_gettime(CLOCK_REALTIME,&now);
	if(rc==0){
		llTime = now.tv_sec* 1000000000+now.tv_nsec;
	}
	else{
		llTime = 0;	
	}
	return llTime;
}
int main(int argc, char ** argv){
	long long int f = 0;
	f = gettime();
	printf("f=%25.2lld\n",f);
	return(0);

}
