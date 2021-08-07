#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
void * handle = NULL,* handle1 = NULL;
int go = 0;
pid_t pd  = 0;
void * try2dlopen(void * data){
    go = 1;
    handle1 = dlopen("libfunc1.so",RTLD_NOW);
    if(handle1 == NULL){
        printf("[%d]dlopen failed.errno:%d\n",pd,errno);
    }else{
        printf("[%d]dlopen libfunc1 done.handle1=%p\n",pd,handle1);
    }

}
void *trydlsym(void * data){
    pd = getpid();
    long tid;
    tid = (long)data;
    int (* pf)();
    printf("[%d/%ld]In onethread().cp0,handle=%p\n",pd,tid,handle);
    do{
        usleep(1000);
    }while(!go && handle==NULL);
    printf("[%d/%ld]In onethread().cp1,handle=%p\n",pd,tid,handle);
    pf = (int (*)())dlsym(handle,"callfunc");
    printf("[%d/%ld]In onethread().cp2,&callfunc=%p\n",pd,tid,pf);
    if(pf == NULL){
        printf("[%d/%ld]func1 failed to dlsym.errno:%d\n",pd,tid,errno);
        return(NULL);
    }
    printf("[%d/%ld]In onethread().cp3,try to call callfunc\n",pd,tid);
    (*pf)();
    printf("[%d/%ld]In onethread().cp9\n",pd,tid);
}

int main(int argc , char * argv[]){
    int rc = 0;
    int num_threads = 20;
    if(argc>1){
        num_threads = atoi(argv[1])+1;
    }
    pthread_t *threads = NULL;
    threads = (pthread_t *)malloc(sizeof(pthread_t)*num_threads);
    pd = getpid();

    printf("[%d]In main().cp0\n",pd);
    handle = dlopen("libfunc2.so",RTLD_NOW);
    if(handle == NULL){
        printf("[%d]dlopen failed.errno:%d\n",pd,errno);
        return(-1);
    }
    
    printf("[%d]In main().cp1\n",pd);
    rc = pthread_create(&threads[0], NULL, try2dlopen, (void *)NULL);
     if (rc){
       printf("[%d]ERROR; return code from pthread_create() for try2dlopen() %d\n", pd,rc);
       exit(-1);
       }
    printf("[%d]In main().cp2\n",pd);
    for(long t=1;t<num_threads;t++){
    //  printf("[%d]In main: creating thread %ld\n",pd, t);
     rc = pthread_create(&threads[t], NULL, trydlsym, (void *)t);
     if (rc){
       printf("[%d]ERROR; return code from pthread_create() for trydlsym() %d\n", pd,rc);
       exit(-1);
       }
    }
    

    printf("[%d]In main().cp3,handle=%p,handle1=%p\n",pd,handle,handle1);
	for(int j = 0;j<num_threads;j++){
		pthread_join(threads[j],NULL);
	}
    printf("[%d]In main().cp9\n",pd);
    dlclose(handle);
    return(0);

}
