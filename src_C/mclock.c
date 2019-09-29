/*
 * main.c
 *
 *  Created on: 2017/8/9
 *      Author: l0087889
 */

#include <sys/timer.h>
#include <sys/resource.h>
#include <sys/systemcfg.h>
#include <time.h>		/* for CLK_TCK (clock ticks per second) */

#include <errno.h>

#include <lib_lock.h>




int main(){

	int rc = 0;

        // struct timespec * tp = (struct timespec *)malloc(sizeof(struct timespec)*10);
	struct timespec tpHost, *tp  = &tpHost;
	/*printf("\nCLOCK_REALTIME = %d,CLOCK_MONOTONIC = %d,CLOCK_PROCESS_CPUTIME_ID = %d,CLOCK_THREAD_CPUTIME_ID = %d",CLOCK_REALTIME,CLOCK_MONOTONIC,CLOCK_PROCESS_CPUTIME_ID,CLOCK_THREAD_CPUTIME_ID);*/
	memset(tp,0,sizeof(struct timespec));

    printf("\nclock_gettime:");
        

	rc = clock_gettime(CLOCK_MONOTONIC, (struct timespec *)tp);
	printf("\nCLOCK_MONOTONIC: rc=%d, tp->tv_sec=%d,tp->tv_nsec=%d",rc,tp->tv_sec,tp->tv_nsec);
	

    printf("\n\n\nclock_getres:");

	memset(tp,0,sizeof(struct timespec));
	rc = clock_getres(CLOCK_MONOTONIC, (struct timespec *)tp);
    printf("\nCLOCK_MONOTONIC: rc=%d,tp->tv_sec=%d,tp->tv_nsec=%d",rc,tp->tv_sec,tp->tv_nsec);

        
	printf("\n");


   return 0;
}
