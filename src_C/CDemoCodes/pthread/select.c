#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
long getTime(){
	struct timeval s;
	gettimeofday(&s,NULL);
	return s.tv_sec*1000*1000 + s.tv_usec;
}
int main(){

	struct timeval s;
	s.tv_sec = 0;
	s.tv_usec = 1000*100;
	long t = getTime();
	select ( 1,NULL,NULL,NULL,&s);
	long diff = getTime()- t;
	printf("diff = %ld\n",diff);
	}
