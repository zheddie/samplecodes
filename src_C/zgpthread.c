#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
typedef struct {
	int threadID;
} ThreadParm;

void * GCThread( void * parm){
	ThreadParm * p = (ThreadParm *)parm;
	printf("p->threadID=%d\n",p->threadID);
}
int main(){
	pthread_t Thd;
	ThreadParm tp;
	tp.threadID=8278;
	pthread_attr_t pta;
	int r=pthread_attr_init(&pta);
	printf("pthread_attr_init=%d\n",r);
	int rc = 11;
	int count = 0;
	while(rc==11){
		rc = pthread_create(&Thd, NULL, GCThread, (void *)&tp);
		count ++;
	}
	pthread_join(Thd,NULL);
	printf("count=%d\n",count);
}
