#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>



typedef struct {
	int threadID;
} ThreadParm;

int iThdCount;

void * DivZero( void * parm){
	ThreadParm * p = (ThreadParm *)parm;

	struct timeval st;
	st.tv_sec = 1;
	st.tv_usec = 0;
	double d1 = 0.0;
	double d2 = 0.0;
	unsigned long i = 0;
	long sum = 0;
	for( i = 0;i<10;i++){
		d2 = d1/(double) 0;
		sum+=i;
		select(NULL,NULL,NULL,NULL,&st);
	}
	printf("zg.DivZero.thread:%d is done,sum=%ld\n",p->threadID,sum);
	pthread_exit(NULL);
}

int main(int argc, char * argv[]){
	iThdCount = 100;
    if(argc > 1){
    	iThdCount = atoi(argv[1]);
    }
    ThreadParm * tp=(ThreadParm *) malloc(sizeof(ThreadParm)*(iThdCount));
    pthread_t * Thd = (pthread_t*) malloc(sizeof(pthread_t)*(iThdCount));

	struct timeval st;
	st.tv_sec = 0;
	st.tv_usec = 10000;
	int rc = 0;
	for(int i=0;i<iThdCount;i++){
		tp[i].threadID = i+1;
		rc = 11;		//EAGAIN
		do{
			rc = pthread_create(&Thd[i], NULL, DivZero, (void *)&tp[i]);
			if(rc == 11){
				printf("zg.MAIN.need wait,i=%d\n",i);
				select(NULL,NULL,NULL,NULL,&st);
			}
		}while(rc==11);
		if (rc){
			printf("zg.MAIN.ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
		//printf("zg.MAIN.thread:%d is created\n",i+1);
	}
	for(int k = 0;k<iThdCount;k++){
		pthread_join(Thd[k],NULL);
	}
}
