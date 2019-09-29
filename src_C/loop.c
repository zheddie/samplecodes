#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc , char * argv[]){
	int sleeptime = 10;
	if(argc>1){
		sleeptime = atoi(argv[1]);
	}
	if(sleeptime <= 0){ sleeptime = 10;}
	for(int i=0;i<sleeptime;i++){
		sleep(1);
		printf("zg.sleep times:%d\n",i);
	}
}
