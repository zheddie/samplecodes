#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>

void (* pdo_once)(bool do_flag);
typedef struct {
	int threadID;
} ThreadParm;

void * GCThread( void * parm){
	ThreadParm * p = (ThreadParm *)parm;
	(*pdo_once)(true);
	printf("p->threadID=%d\n",p->threadID);
}



int fake_main(){

	void * handle = dlopen("_C.so",2);
	/*void * handle = dlopen("_C.so",RTLD_LOCAL|RTLD_NOW);*/
	if(handle == NULL){
		printf("dlopen failed.errno=%d\n",errno);
		return(-1);
	}
	pdo_once = (void (*)(bool))dlsym(handle,"do_once");
	if(pdo_once == NULL){
		printf("do_once failed to dlsym.errno:%d\n",errno);
		return(-1);
	}
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
		}   
	}
	for(int k=0;k<4;k++){

		pthread_join(Thd[k],NULL);
	}
	dlclose(handle);
	return(0);
} 
