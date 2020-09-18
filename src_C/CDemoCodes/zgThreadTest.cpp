#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#define NUM_GCTHREADS 80
#define NUM_WKTHREADS 128
#define TIME_WORKING 24
#define TIME_GC 30
static int gcNum = NUM_GCTHREADS;
static int workNum = NUM_WKTHREADS;
static int workTime = TIME_WORKING;
static int gcTime = TIME_GC;
static pthread_mutex_t gcMtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t tcMtx = PTHREAD_MUTEX_INITIALIZER;	// For TotalCount.
static int iInGC = 0;
static char ** bigString = NULL;
static int TotalCount = 0;
typedef struct {
	int threadID;
} ThreadParm;

long getTime(){
	struct timeval t;
	gettimeofday(&t,NULL);
	return t.tv_sec*1000*1000+t.tv_usec;
}



void * GCThread( void * parm){
	struct timeval slp;

	ThreadParm * p = (ThreadParm *)parm;
	srand(p->threadID);
	long curTime = getTime();
	long endTime = curTime + gcTime * 1000*1000;
	long sum = 0;

	while(curTime < endTime){
		int i = rand() % 1000;
		slp.tv_sec = 0;
		slp.tv_usec = i*1000;
		select(NULL, NULL, NULL, NULL, &slp);//Sleep at most 1 second.
		pthread_mutex_lock(&gcMtx);
		for(int k=0;k<1000*1000;k++){
			sum += k;
		}
		iInGC = 1;
		pthread_mutex_unlock(&gcMtx);
		iInGC = 0;
		curTime = getTime();
	}
	pthread_exit(NULL);
}
void * WorkingThread( void * parm){
	ThreadParm * pp = (ThreadParm *)parm;
	long curTime = getTime();
	long endTime = curTime + workTime * 1000*1000;
	//long sum = 0;
	long count = 0;
	long TimeInWhile = 0;
	while(curTime < endTime){
		if(! iInGC){
			long lTM0 = getTime();
			//pthread_mutex_lock(&gcMtx);
			char * p = (char*) malloc(37);//Combine two string into one.

			for(int k=0;k<1000*10;k++){
				sprintf(p,"%s%s",bigString[k%76800],bigString[76800 - 1 -k%76800]);
			}
			count++;
			//pthread_mutex_unlock(&gcMtx);
			curTime = getTime();
			long diff = curTime - lTM0;
			TimeInWhile += diff;
		}
		else
		{
			while(iInGC){
				struct timeval wGC;
				wGC.tv_sec = 0;
				wGC.tv_usec = 20*1000;
				select(NULL, NULL, NULL, NULL, &wGC);//Sleep at most 1 second.
			}
		}
	}
	pthread_mutex_lock(&tcMtx);
	TotalCount+=count;
	pthread_mutex_unlock(&tcMtx);
	//printf("zg[%03d]TimeInWhile=%ld,Count=%ld\n",p->threadID,TimeInWhile,count);
	pthread_exit(NULL);
}
/* zgThreads <number of gc> < number of working thread> <gc time><workingthreadTime>*/
int main(int argc, char *argv[])
{

    for(int j = 0;j<argc;j++){
    	if(j == 1) gcNum = atoi(argv[1]);
    	if(j == 2) workNum = atoi(argv[2]);
    	if(j == 3) gcTime = atoi(argv[3]);
    	if(j == 4) workTime = atoi(argv[4]);
    }
    printf("gcThreadCount=%d,workingThreadCount=%d,gcThreadTime(s)=%d,workingThreadTime(s)=%d\n",gcNum,workNum,gcTime,workTime);
	ThreadParm * tp=(ThreadParm *) malloc(sizeof(ThreadParm)*(gcNum+workNum));
	pthread_t * Thd = (pthread_t*) malloc(sizeof(pthread_t)*(gcNum+workNum));
	int rc;
	printf("zg.MAIN.Entry\n");
	struct timeval st;
	st.tv_sec = 0;
	st.tv_usec = 1000;
	struct timeval st4;
	st4.tv_sec = 1;
	if(gcTime - workTime -1 > 0){
		st4.tv_sec = gcTime - workTime -1;
	}
	st4.tv_usec = 0;
	long lTM0=getTime();
	bigString = (char **)malloc(76800*sizeof(char *));
	for (int x =0;x<76800;x++){
		bigString[x] = (char *)malloc(19);

		sprintf(bigString[x],"%s%d","3602912687245",x);
		//printf("bigSting[%d]=%s",x,bigString[x]);
	}
	for(int i=0;i<(gcNum+workNum);i++){
		tp[i].threadID = i+1;
		if(i == gcNum){
			select(NULL,NULL,NULL,NULL,&st4);
		}
		rc = 11;		//EAGAIN
		do{
			if(i < gcNum){
				rc = pthread_create(&Thd[i], NULL, GCThread, (void *)&tp[i]);
			}
			else{
				rc = pthread_create(&Thd[i], NULL, WorkingThread, (void *)&tp[i]);
			}
			if(rc == 11){
				printf("zg.MAIN.need wait\n");
				select(NULL,NULL,NULL,NULL,&st);
			}
		}while(rc==11);
		if (rc){
			printf("zg.MAIN.ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
		//printf("zg.MAIN.thread:%d is created\n",i+1);
	}
	long diff = getTime() -lTM0;
	printf("zg.MAIN.Create thread used:%ld\n",diff);
	for(int k = 0;k<(gcNum+workNum);k++){
		pthread_join(Thd[k],NULL);
	}

	printf("zg.MAIN.exit.avgCount=%d\n",TotalCount/workNum);
	/* Last thing that main() should do */
	pthread_exit(NULL);
}
