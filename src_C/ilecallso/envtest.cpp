#include <stdio.h>
#define _POSIX_SOURCE
#include <stdlib.h>
int main(){
    char * penv = NULL;
    penv = getenv("LIBPATH");
    printf("zg.LIBPATH=%s\n",penv);

    putenv("LIBPATH=/QOpenSys/QIBM/ProdData/JavaVM/jdk80/64bit/jre/lib/ppc64/");
    penv = getenv("LIBPATH");
    printf("zg.LIBPATH=%s\n",penv);
    return(0);
}