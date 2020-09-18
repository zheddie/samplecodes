#include <stdio.h>
#include <string.h>
#include <time.h>
void main(int argc, char ** argv){
	long secs = 0;
	char ss[20];
	if(argc >1) {
		secs = atol(argv[1]);
		}
/*	if(argc>1){
		usecs = atoi(argv[2]);
		}
	struct timeval t;
	t.tv_sec = secs;
	t.tv_usec = usecs;
*/
	memset(ss,0,20);
	ctime_r((time_t*)&secs,ss);
	printf("time=%s\n",ss);
	

}
