#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>


#define NUM_WKTHREADS 128
#define TIME_WORKING 3
static int workNum = NUM_WKTHREADS;
static int workTime = TIME_WORKING;
static int HeapSizeG =  10;
static pthread_mutex_t tcMtx = PTHREAD_MUTEX_INITIALIZER;	// For TotalCount.
static char ** bbString = NULL;
//static char * bbString1 = NULL;
static int TotalCount = 0;
static int waitTime = 10;
typedef struct {
	int threadID;
	int wTime;
} ThreadParm;

unsigned long getTime(){
	struct timeval t;
	gettimeofday(&t,NULL);
	return t.tv_sec*1000*1000+t.tv_usec;
}

/*exchange 1024 characters within the string p.*/
int exchange1K(char * p,int firstK,int secondK){
	int exchangeSize = 1024;
	int start1 = firstK*1024;
	int start2 = secondK*1024;
	if(firstK<0 || secondK < 0){
		return (-1);
	}
	if(firstK=secondK){  //let's do 0.5K exchange.
		exchangeSize=512;
		start2 = start1+exchangeSize;
	}
	char * ps = (char *)malloc(exchangeSize+1);
	memset(ps,0,exchangeSize+1);
	char tmp1 = p[start1+exchangeSize-1];
	p[start1+exchangeSize-1] = '\0';
	char tmp2 = p[start2+exchangeSize-1];
	p[start1+exchangeSize-1] = '\0';
	strcpy(ps,&p[start1]);
	strcpy(&p[start1],&p[start2]);
	strcpy(&p[start2],ps);
	free(ps);
	return(0);
}

void * WorkingThread( void * parm){
	ThreadParm * pp = (ThreadParm *)parm;
	long curTime = getTime();
	long endTime = curTime + pp->wTime * 1000*1000;
	//long sum = 0;
	long count = 0;
	long TimeInWhile = 0;
	while(curTime < endTime){
			long lTM0 = getTime();
			//pthread_mutex_lock(&gcMtx);
			//Real work hear, let's just read some thing from the big big string.
			int bbIdxX = 0;
			int bbIdxY = 0;
			int bbIdxS = 0;



				bbIdxX = random()% 1024;
				bbIdxY = random()% 1024;

				struct timeval st;
				st.tv_sec = 0;
				int iWaitUS = random()%waitTime;
				st.tv_usec = iWaitUS;
				select(NULL,NULL,NULL,NULL,&st);
				//bbIdxS = random()% 1024*HeapSizeG;
				//Crate a new string.
				char * p = (char *)malloc(1024*HeapSizeG+1);
				memset(p,'0',1024*HeapSizeG+1);
//				for(int tt = 0;tt<1024*HeapSizeG;tt++){
//					char r = random()% 10+ '0';
//					p[tt] = r;
//				}
				strcpy(bbString[bbIdxX*bbIdxY],p);
				free(p);
				bbIdxS = random()% HeapSizeG;
				exchange1K(&(bbString[bbIdxX*bbIdxY][0]),bbIdxS,HeapSizeG-1);

			//printf("X=%d,Y=%d,S=%d\n",bbIdxX,bbIdxY,bbIdxS);
			count++;
			//pthread_mutex_unlock(&gcMtx);
			curTime = getTime();
			long diff = curTime - lTM0;
			TimeInWhile += diff;
	}
	pthread_mutex_lock(&tcMtx);
	TotalCount+=count;
	pthread_mutex_unlock(&tcMtx);
	//printf("zg[%03d]TimeInWhile=%ld,Count=%ld\n",p->threadID,TimeInWhile,count);
	pthread_exit(NULL);
}
/* zgHeap < number of working thread> <workingthreadTime> <heapsize in G> <wait time for each action in usec>*/
int main(int argc, char *argv[])
{

    for(int j = 0;j<argc;j++){
    	if(j == 1) workNum = atoi(argv[1]);
    	if(j == 2) workTime = atoi(argv[2]);
    	if(j == 3) HeapSizeG = atoi(argv[3]);
    	if(j == 4) waitTime = atoi(argv[4]);
    }
    printf("ThreadCount=%d,warmupTime(s)=%d,measureTime=%d,HeapSize=%d(G),waitTime=%d\n",workNum,workTime,workTime*8,HeapSizeG,waitTime);
	ThreadParm * tp=(ThreadParm *) malloc(sizeof(ThreadParm)*(workNum));
	pthread_t * Thd = (pthread_t*) malloc(sizeof(pthread_t)*(workNum));
	int rc;
	printf("zg.MAIN.Entry\n");
	struct timeval st;
	st.tv_sec = 0;
	st.tv_usec = 1000;
	unsigned long lTM0=getTime();
	srandom(lTM0);
	//Now we have a 1024 *1024 strings, and each string has 10k size. So, we total have 10G memory.
//	bbString1 = (char *)malloc(1024*1024*1024*HeapSizeG+1);
//	memset(bbString1,0,HeapSizeG*1024*1024*1024+1);
//	for(int y = 0;y<((HeapSizeG*1024*1024*1024)/10);y++){
//		sprintf(bbString1,"%s%s",bbString1,"0123456789");
//	}
	bbString = (char **)malloc(1024*1024*sizeof(char *));
	for (int x =0;x<1024*1024;x++){
		bbString[x] = (char *)malloc(HeapSizeG*1024+1);
		memset(bbString[x],0,HeapSizeG*1024+1);
		for(int tt = 0;tt<1024*HeapSizeG;tt++){
			char r = random()% 10+ '0';
			bbString[x][tt] = r;
		}
//		for(int y = 0;y<((HeapSizeG*1024)/10);y++){
//			sprintf(bbString[x],"%s%s",bbString[x],"0123456789");
//		}
	}
	unsigned long lTM1 = getTime();
	long diff = lTM1 -lTM0;
	printf("zg.MAIN.Create big Strings used:%ld\n",diff);
	int warmNum = workNum /2 ;
	for(int whIdx = 1;whIdx<=workNum;whIdx++){

		TotalCount = 0;//We need to clear the total count for a new warehouse.

		lTM0 = getTime();
		int localWorkTime = 0;
		if(whIdx > warmNum){//We finsihed the warmUp, we need set meauser time.
			localWorkTime = workTime*1;
		}else{
			localWorkTime = workTime;
		}
		for(int i=0;i<(whIdx);i++){
			tp[i].threadID = i+1;
			tp[i].wTime = localWorkTime;
			rc = 11;		//EAGAIN
			do{
					rc = pthread_create(&Thd[i], NULL, WorkingThread, (void *)&tp[i]);
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
//		diff = getTime() -lTM0;
//		printf("zg.MAIN.Create thread used:%ld\n",diff);
		for(int k = 0;k<(whIdx);k++){
			pthread_join(Thd[k],NULL);
		}

		//printf("zg.MAIN.exit.avgCount=%d\n",TotalCount/workNum);
		printf("zg.MAIN.whIdx=%d,TotalCount/sec=%d\n",whIdx,TotalCount/localWorkTime);
	}
	/* Last thing that main() should do */
	pthread_exit(NULL);
}
