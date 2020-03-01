#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#define NUM_THREADS 20
#define NUM_CREATORS 50
#define NUM_SPEED 10
#define NUM_MAXWAIT 50
static int creatorNum = NUM_CREATORS;



typedef struct {
	int threadID;
	int waitTime;
	int workNumber;
} creatorThreadParm;

void * CreatorThread( void * parm){
	char s = 'x';
	creatorThreadParm *p = (creatorThreadParm *)parm;
	int creatorID = p->threadID;
	printf("zg.CreatorThread[%d],&s=%p,p=%p,&creatorID=%p\n",creatorID,&s,p,&creatorID);
	for(int index=0;index<5;index++){
		sleep(5);
	}
	printf("zg.CreatorThread[%d].exit\n",creatorID);
	pthread_exit(NULL);

}
/* zgThreads <number of creators> < number of threads> <create speed> <wait time for one thread>*/
int main(int argc, char *argv[])
{
	int rc;
	long t;
	creatorThreadParm ctp[creatorNum];
	pthread_t creatorThd[creatorNum];


	struct timeval st;
	st.tv_sec = 0;
	st.tv_usec = 100*1000;
	printf("zg.MAIN.creatorThread=%p,&rc=%p,&t=%p\n",CreatorThread,&rc,&t);
	for(t=0;t<creatorNum;t++){
		ctp[t].threadID = t;
		//printf("zg.MAIN: creating thread %ld\n", t);
		rc = 11;
		int eagaincount = -1;
		while(rc==11){
			rc = pthread_create(&creatorThd[t], NULL, CreatorThread, (void *)&ctp[t]);
			select(1,NULL,NULL,NULL,&st);
			eagaincount++;
			}
		if(eagaincount ) printf("zg.MAIN.EAGAINcount =%d\n", eagaincount);

	}
	for(int j = 0;j<creatorNum;j++){
		pthread_join(creatorThd[j],NULL);
	}
	printf("zg.MAIN.exit\n");
	/* Last thing that main() should do */
	pthread_exit(NULL);
}
