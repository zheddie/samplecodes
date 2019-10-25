#include <stdlib.h>
#include <dlfcn.h>
#include <errno.h>
int (* pf)(char * , char *);

int main(){
    
    //void * handle = dlopen("libfunc1.so",RTLD_NOW);
    putenv("LIBPATH=/qopensys/home/zhanggan/codes/");
    void * handle = dlopen("libpasefn.so",RTLD_NOW);
    if(handle == NULL){
        printf("dlopen failed.errno:%d\n",errno);
        return(-1);
    }
    pf = (int (*)())dlsym(handle,"paseFunction");
    if(pf == NULL){
     printf("func1 failed to dlsym.errno:%d\n",errno);
    return(-1);
    }
    char input[]="zinput";
    char output[20];
    (*pf)(input,output);
    printf("input=%s,output=%s\n",input,output);
    dlclose(handle);

    return(0);

}
