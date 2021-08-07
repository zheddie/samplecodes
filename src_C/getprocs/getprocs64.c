#include <sys/procfs.h>
#include <procinfo.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define BUFFSIZE 100
int main(){
    struct procentry64 procBuf[BUFFSIZE];
    //struct fdsinfo64 fileBuf[BUFFSIZE];
    int rCount=0,idx=0;
    pid_t *pidx = (pid_t *)malloc(sizeof(pid_t));
    *pidx = 0;

    for(int i = 0; i < 1000* 1000; i++){
        //    start = clock();
        do{
            printf( "****pidx0：%ld********\n", *pidx);          
            rCount = getprocs64(procBuf,sizeof(struct procentry64),NULL,0,pidx,BUFFSIZE);
            // printf( "pidx：%ld\n", *pidx);
            // printf( "add_pidx：%x\n", pidx);
            printf("rCount : %d:", rCount);
            for(idx=0;idx<rCount;idx++ ){
                printf(" %d",procBuf[idx].pi_pid);
                }
            printf("\n");
        }while(rCount > 0);
        
        memset(procBuf,0, 100);
        *pidx = 0;
        printf("Round(%d) done.\n",i);
        usleep(10*1000);
    }
}