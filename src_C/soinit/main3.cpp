#include <iostream>
#include <dlfcn.h>
#include <errno.h>
int (* pf)();

int main(){
    
    //void * handle = dlopen("libfunc1.so",RTLD_NOW);
    //putenv("LIBPATH=/QOpenSys/home/zhanggan/pytorch/build/lib.os400-powerpc64-3.6/torch");
    void * handle = dlopen("_C.so",RTLD_NOW);
    if(handle == NULL){
        std::cout<<"dlopen failed.errno:"<<errno<<std::endl;
        return(-1);
    }

    dlclose(handle);

    return(0);

}
