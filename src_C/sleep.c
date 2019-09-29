#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char * argv[]){
	int sleeptime=10;
	if(argc >1){
		sleeptime  = atoi(argv[1]);
	}
	sleep(sleeptime);
	return(0);
}
