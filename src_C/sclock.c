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

void sig_handler(int signo)
{
  if (signo == SIGILL)
    printf("**ILLEGAL INSTRUCTION HIT**");
}
long long clockids[4][1] = {{CLOCK_REALTIME},{CLOCK_MONOTONIC},{CLOCK_PROCESS_CPUTIME_ID},{CLOCK_THREAD_CPUTIME_ID}};
char clockids_name[4][30] = {"CLOCK_REALTIME","CLOCK_MONOTONIC","CLOCK_PROCESS_CPUTIME_ID","CLOCK_THREAD_CPUTIME_ID"};
typedef int (* CLOCKFP)(long long,struct timespec *);
CLOCKFP sp[2]={clock_gettime,clock_getres};

int main(){

	int rc = 0;
	if (signal(SIGILL, sig_handler) == SIG_ERR)
  		printf("\ncan't catch SIGINT\n");
        // struct timespec * tp = (struct timespec *)malloc(sizeof(struct timespec)*10);
	struct timespec tpHost, *tp  = &tpHost;
	for(int j=0;j<2;j++){
		if(j==0)
			printf("\nclock_gettime(EINTR=%d):",EINTR);
		else
			printf("\nclock_getres(EINTR=%d):",EINTR);
		for(int i =0;i<4;i++){
			printf("\n%s: ",(char *)(clockids_name[i]));
			memset(tp,0,sizeof(struct timespec));
			rc = sp[j](clockids[i][0], tp);
			printf("rc=%d,errno=%d,tp->tv_sec=%d,tp->tv_nsec=%d",rc,errno,tp->tv_sec,tp->tv_nsec);
		}
	}
	

  
	printf("\n");


   return 0;
}
