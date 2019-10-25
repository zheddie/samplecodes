#define _LARGE_FILES
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
// #define lseek lseek64
// #define off_t off64_t
#define BLOCKSIZE (1024*1024)
int main(){
    //long location = 1640109465L;
    off_t ort = 0;
    int irt = 0;
    unsigned long size = 2840109465L;
    //+ 1540108465L;
    umask(S_IWGRP | S_IWOTH);
    int fp = open("zlargefile.dat",O_RDWR|O_CREAT);
    char data[BLOCKSIZE] ;
    memset(data,'e',BLOCKSIZE);
    write(fp,data,BLOCKSIZE);
    close(fp);
    fp = open("zlargefile.dat",O_RDWR);
    int current_block =   1;
    int last_block =  (size / BLOCKSIZE)
                            + ((size % BLOCKSIZE) != 0);
    printf("zg.last_block=%d\n",last_block);
    for (current_block =1 ; current_block < last_block ; current_block++) {
        ort  = lseek(fp,(off_t)(current_block*BLOCKSIZE),SEEK_SET);
        if (ort < 0) {
            printf("zg.lseek failed with ort=%llu,at %d,errno=%d\n",ort,current_block,errno);
            close(fp);
            return -1;
        }
        irt = write(fp,"\0",1);
        if (irt < 0) {
            printf("zg.write failed with irt=%d,at %d,errno=%d\n",irt,current_block,errno);
            close(fp);
            return -1;
        }
        printf("zg.current_block=%d\n",current_block);
    } 
    close(fp);
    printf("zg.successfully done.\n");

}