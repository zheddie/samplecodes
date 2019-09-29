#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>

int getopsidx();

int main(){
    // dlclose(handle);
    int idx=getopsidx();
    printf("helloworld=%d\n",idx);
    return(0);

}
