#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct {
	int threadID;
} ThreadParm;

void * GCThread( void * parm){
	ThreadParm * p = (ThreadParm *)parm;
	printf("p->threadID=%d\n",p->threadID);
}



int main(){

	pthread_t Thd[4];
	pthread_attr_t pta;
	int r=pthread_attr_init(&pta);
	printf("pthread_attr_init=%d\n",r);
	for(int j=0;j<4;j++){
		ThreadParm tp; 
		tp.threadID=j;
		int rc = 11; 
		while(rc==11){
			rc = pthread_create(&Thd[j], NULL, GCThread, (void *)&tp);
			sleep(1);
		}   
	}
	for(int k=0;k<4;k++){

		pthread_join(Thd[k],NULL);
	}
	return(0);
} 
