#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#define NUM_THREADS 10
#define NUM_CREATORS 10
#define NUM_SPEED 10
#define NUM_MAXWAIT 20
static int creatorNum = NUM_CREATORS;
static int workNum = NUM_THREADS;
static int speedNum = NUM_SPEED;
static int maxWaitTime = NUM_MAXWAIT;
static int createdThreads = 0;
static int endedThreads = 0;
static int startedThreads = 0;
static pthread_mutex_t strMtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t endMtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t crtMtx = PTHREAD_MUTEX_INITIALIZER;
static int mainExit = 0;
typedef struct {
	int threadID;
	int creatorID;
	int heapSize;
} workThreadParm;

typedef struct {
	int threadID;
	int waitTime;
	int workNumber;
} creatorThreadParm;
long getTime(){
	struct timeval t;
	gettimeofday(&t,NULL);
	return t.tv_sec*1000*1000+t.tv_usec;
}

void * MonitorThread(void * p){
	while(!mainExit){
		long t = getTime();
		for(int a =0;a<5;a++){
			printf("^");
			sleep(1);
		}
		printf("\n");
		long diff = (getTime() - t)/(1000*1000);
		if(diff>5) printf("zg.MONITOR.ERROR.diff=%ld\n",diff);
		printf("zg.MONITOR.created=%d,ended=%d,started=%d\n",createdThreads,endedThreads,startedThreads);
	}
}
void * WorkThread(void * parm){
	srand((unsigned)time(0));	
	workThreadParm *p = (workThreadParm *)parm;
	pthread_mutex_lock(&strMtx);
	startedThreads++;
	pthread_mutex_unlock(&strMtx);	
	int wTime = (random()%maxWaitTime)/5;
	if(wTime > 5) printf("zg.WorkThread,wTime=%d\n",wTime);
#ifdef DEBUG
	printf("zg.WorkThread.creator=%d,num=%d,waitTime=%d\n",p->creatorID,p->threadID,wTime);
#endif
	for(int index=0;index<5;index++){
		sleep(wTime);
	}
	pthread_mutex_lock(&endMtx);
	endedThreads++;
	pthread_mutex_unlock(&endMtx);
	pthread_detach(pthread_self());
	pthread_exit(NULL);

}

void * CreatorThread( void * parm){
	pthread_t workThd[workNum];
	workThreadParm wtp[workNum];
	int interval = 1000/speedNum;
	struct timeval intv;
	intv.tv_sec = 0;
	intv.tv_usec = interval*200; 
	int rc = 0;
	creatorThreadParm *p = (creatorThreadParm *)parm;
#ifdef DEBUG
	printf("zg.CreatorThread:creatorNum=%d,interval=%d\n",p->threadID,interval);
#endif
	int creatorID = p->threadID;
	for(int k=0;k<workNum;k++){
		wtp[k].threadID = k;
		wtp[k].creatorID = creatorID;
		rc = 11; //EAGAIN
		int eagaincount = -1;
		while(rc == 11){
			rc = pthread_create(&workThd[k],NULL,WorkThread,(void *) & wtp[k]);
			//select(1,NULL,NULL,NULL,&intv);
			//sleep(1);
			eagaincount++;
			if(eagaincount > 0 && 0 == eagaincount % 10000 ) printf("zg.CreatorThread[%d][%d],eagaincount=%d\n",creatorID,k,eagaincount);
			}
		//sleep(1);
		if(eagaincount) printf("zg.CreatorThread[%d].EAGAIN count = %d\n",creatorID,eagaincount);

		//select(1,NULL,NULL,NULL,&intv);
		pthread_mutex_lock(&crtMtx);
		createdThreads++;
		pthread_mutex_unlock(&crtMtx);
	}
/*
	for(int s=0;s<workNum;s++){
		pthread_join(workThd[s],NULL);
	}
*/
	pthread_exit(NULL);
}
/* zgThreads <number of creators> < number of threads> <create speed> <wait time for one thread>*/
int main(int argc, char *argv[])
{
	for(int j = 0;j<argc;j++){
		if(j == 1) creatorNum = atoi(argv[1]);
		if(j == 2) workNum = atoi(argv[2]);
		if(j == 3) speedNum =  atoi(argv[3]);
		if(j == 4) maxWaitTime = atoi(argv[4]);
	}
	int pid = getpid();
	printf("zg.MAIN(%d).creatorNum=%d,workNum=%d,speedNum=%d,maxWaitTime=%d\n",pid,creatorNum,workNum,speedNum,maxWaitTime);
	creatorThreadParm ctp[creatorNum];
	pthread_t creatorThd[creatorNum];
	int rc;
	long t;
	
	pthread_t monitorThd ;
	rc = 11;
	while(rc == 11){
		rc = pthread_create(&monitorThd,NULL,MonitorThread,NULL);
	}
	if(rc){
		printf("zg.MAIN.monitor thread create failed.rc=%d\n",rc);
		exit(-1);
	}
	struct timeval st;
	st.tv_sec = 0;
	st.tv_sec = 100*1000;
	for(t=0;t<creatorNum;t++){
		ctp[t].threadID = t;
		//printf("zg.MAIN: creating thread %ld\n", t);
		rc = 11;
		int eagaincount = -1;
		while(rc==11){
			rc = pthread_create(&creatorThd[t], NULL, CreatorThread, (void *)&ctp[t]);
			//sleep(1);
			eagaincount++;
			}
		if(eagaincount ) printf("zg.MAIN.EAGAINcount =%d\n", eagaincount);
	//	select(1,NULL,NULL,NULL,&st);
		//sleep(1);
	}
	for(int j = 0;j<creatorNum;j++){
		pthread_join(creatorThd[j],NULL);
	}
	mainExit = 1;
	printf("zg.MAIN.exit\n");
	/* Last thing that main() should do */
	pthread_exit(NULL);
}
