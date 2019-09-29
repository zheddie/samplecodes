#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>
int (* pf)();

int main(){
    
    void * handle = dlopen("libfunc2.so",RTLD_NOW);
    //putenv("LIBPATH=/QOpenSys/home/zhanggan/pytorch/build/lib.os400-powerpc64-3.6/torch");
    //void * handle = dlopen("_C.so",RTLD_NOW);
    if(handle == NULL){
        printf("dlopen failed.errno:%d\n",errno);
        return(-1);
    }

    dlclose(handle);

    return(0);

}
