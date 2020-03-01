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

	static int sleepTimeSec = 0;
	static int sleepTimeuSec = 5000;
	static int iTimes = 3;
	static int iMaxCount = 10;
	static int iThreadNumber = 10;
	typedef struct {
		int threadID;
	} ThreadParm;

unsigned long long getTime(){
	struct timeval t;
	gettimeofday(&t,NULL);
	return t.tv_sec*1000*1000+t.tv_usec;
}
void * getTimeLoop(void * parm){
	ThreadParm * pp = (ThreadParm *)parm;
	int count=0;
	int errCount = 0;
	unsigned long long lTM0=0,lTM1=0;
	long diff=0;
	long errDiff = 0;
	struct timeval st1;
	st1.tv_sec = sleepTimeSec;
	st1.tv_usec = sleepTimeuSec; //5 millis

	do{


		lTM0=getTime();

		select(NULL,NULL,NULL,NULL,&st1);
		//sleep(sleepTime);
		count ++;
		lTM1=getTime();

		diff = lTM1 - lTM0;
		if(diff > iTimes*(sleepTimeSec*1000*1000+sleepTimeuSec) || diff < 0 ){
			//printf("zg.iTimes*(sleepTimeSec*1000*1000+sleepTimeuSec=%llu,(sleepTimeSec*1000*1000+sleepTimeuSec)/10=%llu\n",iTimes*(sleepTimeSec*1000*1000+sleepTimeuSec),(sleepTimeSec*1000*1000+sleepTimeuSec)/10);
			errDiff = diff;
			printf("ERROR.[%d]TimeDiff=%ld,count=%d,errCount=%d,start=%llu,now=%llu\n",pp->threadID, diff,count,errCount,lTM0,lTM1);
			errCount++;
			if(errCount > iMaxCount){break;}
		}else{
//			if(count % (5*1000*1000/(sleepTimeSec*1000*1000+sleepTimeuSec)) == 0){//print out date every 15 seconds.
//				printf("TimeDiff=%ld,count=%d,errCount=%d,errDiff=%ld\n",diff/1000,count,errCount,errDiff/1000);
//			}
		}
	}while(1);
	return (NULL);
}
/*zgGetTime <millis count for Sleep> <Times for diff to print out> <Max err count for stop>*/
int main(int argc, char * argv[]){

	struct timeval st;
	if(argc > 1){
		sleepTimeSec = atoi(argv[1])/1000;
		sleepTimeuSec = (atoi(argv[1]) % 1000) * 1000;
	}

	if(argc > 2){
		iTimes = atoi(argv[2]);
	}

	if(argc > 3){
		iMaxCount = atoi(argv[3]);
	}
	printf("zg.sleepTimeSec=%d,sleepTimeuSec=%d,iTimes=%d,iMaxCount=%d\n",sleepTimeSec,sleepTimeuSec,iTimes,iMaxCount);
	ThreadParm * tp=(ThreadParm *) malloc(sizeof(ThreadParm)*(iThreadNumber));
	pthread_t * Thd = (pthread_t*) malloc(sizeof(pthread_t)*(iThreadNumber));
	st.tv_sec = 1;
	st.tv_usec = 1000;//100 ms
	int cc = 0;
	for (int i =0;i<iThreadNumber;i++){
		tp[i].threadID=i;
		int rc = 11;
		do{
			rc = pthread_create(&Thd[i], NULL, getTimeLoop, (void *)&tp[i]);
			if(rc == 11){
				cc++;
				printf("zg.MAIN.need wait,cc=%d\n",cc);
				select(NULL,NULL,NULL,NULL,&st);
			}
		}while(rc==11);
		if (rc){
			printf("zg.MAIN.ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}
	for(int k = 0;k<iThreadNumber;k++){
		pthread_join(Thd[k],NULL);
	}
	pthread_exit(NULL);
}
