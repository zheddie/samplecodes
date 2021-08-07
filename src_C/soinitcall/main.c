#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>

int (* pf)();

int main(){
    pid_t pd = getpid();
    printf("[%d]In main().cp0\n",pd);
    void * handle = dlopen("libfunc1.so",RTLD_NOW);
    if(handle == NULL){
        printf("[%d]dlopen failed.errno:%d\n",pd,errno);
        return(-1);
    }
    pd = getpid();
    printf("[%d]In main().cp1\n",pd);
    pf = (int (*)())dlsym(handle,"callfunc");
    printf("[%d]In main().cp2\n",pd);
    if(pf == NULL){
     printf("[%d]func1 failed to dlsym.errno:%d\n",pd,errno);
    return(-1);
    }
    printf("[%d]In main().cp3\n",pd);
    (*pf)();
    printf("[%d]In main().cp4\n",pd);
    dlclose(handle);

    return(0);

}
