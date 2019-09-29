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

#include <iostream>
#include <exception>



#include <errno.h>

#include <lib_lock.h>


using namespace std;

int main(){

	int rc = 0;

        // struct timespec * tp = (struct timespec *)malloc(sizeof(struct timespec)*10);
	struct timespec tpHost, *tp  = &tpHost;
	/*printf("\nCLOCK_REALTIME = %d,CLOCK_MONOTONIC = %d,CLOCK_PROCESS_CPUTIME_ID = %d,CLOCK_THREAD_CPUTIME_ID = %d",CLOCK_REALTIME,CLOCK_MONOTONIC,CLOCK_PROCESS_CPUTIME_ID,CLOCK_THREAD_CPUTIME_ID);*/
	memset(tp,0,sizeof(struct timespec));

    printf("\nclock_gettime:");
        
	rc = clock_gettime(CLOCK_REALTIME, (struct timespec *)tp);
	printf("\nCLOCK_REALTIME:rc=%d, tp->tv_sec=%d,tp->tv_nsec=%d",rc,tp->tv_sec,tp->tv_nsec);
	
	memset(tp,0,sizeof(struct timespec));
	rc = clock_gettime(CLOCK_MONOTONIC, (struct timespec *)tp);
	printf("\nCLOCK_MONOTONIC: rc=%d, tp->tv_sec=%d,tp->tv_nsec=%d",rc,tp->tv_sec,tp->tv_nsec);
	

    /* This is not supported on i*/
    
    
    memset(tp,0,sizeof(struct timespec));
    try{
		rc = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, (struct timespec *)tp);
	    printf("\nCLOCK_PROCESS_CPUTIME_ID: rc=%d,tp->tv_sec=%d,tp->tv_nsec=%d",rc,tp->tv_sec,tp->tv_nsec);
	}
	catch(exception &e)
	{
		printf("\nCLOCK_PROCESS_CPUTIME_ID:Illegal Instruction failue");
	}
	//printf("\nCLOCK_THREAD_CPUTIME_ID:Illegal Instruction failue");
	memset(tp,0,sizeof(struct timespec));
	rc = clock_gettime(CLOCK_THREAD_CPUTIME_ID, (struct timespec *)tp);
	printf("\nCLOCK_THREAD_CPUTIME_ID: rc=%d,tp->tv_sec=%d,tp->tv_nsec=%d",rc,tp->tv_sec,tp->tv_nsec);
	


    printf("\n\n\nclock_getres:");


	memset(tp,0,sizeof(struct timespec));
	rc = clock_getres(CLOCK_THREAD_CPUTIME_ID, (struct timespec *)tp);
    printf("\nCLOCK_THREAD_CPUTIME_ID: rc=%d, tp->tv_sec=%d,tp->tv_nsec=%d",rc,tp->tv_sec,tp->tv_nsec);
	printf("\nCLOCK_MONOTONIC:Illegal Instruction failue");
	printf("\nCLOCK_PROCESS_CPUTIME_ID:Illegal Instruction failue");
	printf("\nCLOCK_REALTIME:Illegal Instruction failue");

	/*

    memset(tp,0,sizeof(struct timespec));    
	rc = clock_getres(CLOCK_REALTIME, (struct timespec *)tp);
    printf("\nCLOCK_REALTIME: rc=%d,tp->tv_sec=%d,tp->tv_nsec=%d",rc,tp->tv_sec,tp->tv_nsec);
	*/
	/*(memset(tp,0,sizeof(struct timespec));
	rc = clock_getres(CLOCK_MONOTONIC, (struct timespec *)tp);
    printf("\nCLOCK_MONOTONIC: rc=%d,tp->tv_sec=%d,tp->tv_nsec=%d",rc,tp->tv_sec,tp->tv_nsec);
	
	memset(tp,0,sizeof(struct timespec));
	rc = clock_getres(CLOCK_PROCESS_CPUTIME_ID, (struct timespec *)tp);
    printf("\nCLOCK_PROCESS_CPUTIME_ID: rc=%d,tp->tv_sec=%d,tp->tv_nsec=%d",rc,tp->tv_sec,tp->tv_nsec);
	
	memset(tp,0,sizeof(struct timespec));
	rc = clock_getres(CLOCK_THREAD_CPUTIME_ID, (struct timespec *)tp);
    printf("\nCLOCK_THREAD_CPUTIME_ID: rc=%d, tp->tv_sec=%d,tp->tv_nsec=%d",rc,tp->tv_sec,tp->tv_nsec);
	*/
        
	printf("\n");


   return 0;
}
