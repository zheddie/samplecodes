/*
 * client.cpp
 *
 *  Created on: Jan 3, 2017
 *      Author: zhanggan
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MEMSIZE 1024*1024

int main(int argc, char *argv[])
{


    union {
    		void *address;
    		char chars[sizeof(void *)] ;
    };
    for (int i =1;i<32;i++){
    		address = (void *)malloc(MEMSIZE*i);
    	    printf("zg.address[%dm]=0x",i);
    	    for(int zi  = sizeof(void *)-1;zi>-1;zi--){
    	    		printf("%02X",chars[zi] & 0xFF);
    	    }
    	    free(address);
    	    printf("\n");
    }
    return 0;
}


