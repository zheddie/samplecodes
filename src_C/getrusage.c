/*
 * getrusage.c
 *
 *  Created on: Jul 21, 2014
 *      Author: zhanggan
 */

#include <stdlib.h>

#include <stdio.h>
#include <sys/times.h>
#include <sys/resource.h>


int main(){
	struct rusage u;
	long rtnvalue=0;
	long index = 0;

	for(index = 0; index < 1000*1000;index++){
		if(index % 10000 == 0){
			printf("zg.index=%ld\n",index);
		}
	}
	if (0 == getrusage(RUSAGE_SELF, &u)) {
		rtnvalue = (u.ru_utime.tv_sec + u.ru_stime.tv_sec) * 10000000 +
			(u.ru_utime.tv_usec  + u.ru_stime.tv_usec) * 10;
	}
	printf("zg.rtnvalue=%ld\n",rtnvalue);


}
