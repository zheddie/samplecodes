#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
/*
 * zgGetTime.cpp
 *
 *  Created on: Jan 28, 2014
 *      Author: zhanggan
 */

long getTime(){
	struct timeval t;
	gettimeofday(&t,NULL);
	return t.tv_sec*1000*1000+t.tv_usec;
}
int main(int argc, char * argv[]){

	long long s1  = getTime();
	printf("s1=%lld\n",s1);
	long long s2  = getTime();
	printf("s1=%llu,s2=%lld\n",s1,s2);
}
