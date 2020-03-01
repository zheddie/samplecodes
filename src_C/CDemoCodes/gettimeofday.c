#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

long long getTime(){
	struct timeval t;
	gettimeofday(&t,NULL);
	return t.tv_sec*1000*1000+t.tv_usec;
}

int main(int argc, char *argv[])
{
	long long t = getTime();
	printf("zg.getTime=%lld\n",t);
}
