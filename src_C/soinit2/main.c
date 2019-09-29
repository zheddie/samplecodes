#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>
int (* pf)();

int main(){
    
    void * handle = dlopen("libfunc1.so",RTLD_NOW);
    //putenv("LIBPATH=/QOpenSys/home/zhanggan/pytorch/build/lib.os400-powerpc64-3.6/torch");
    //void * handle = dlopen("_C.so",RTLD_NOW);
    if(handle == NULL){
        printf("dlopen failed.errno:%d\n",errno);
        return(-1);
    }
    pf = (int (*)())dlsym(handle,"callfunc2");
    if(pf == NULL){
     printf("func1 failed to dlsym.errno:%d\n",errno);
    return(-1);
    }
    (*pf)();

    dlclose(handle);

    return(0);

}
