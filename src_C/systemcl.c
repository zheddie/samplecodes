/*
 * systemcl.c
 *
 *  Created on: May 15, 2017
 *      Author: zhanggan
 */


#include <as400_protos.h>
#include <stdio.h>
#include <errno.h>
int main(int argc, char * argv[]){
	int idx = 0;
	int rtn = 0;
	if(argc < 2){
		printf("Usage:\n\t%s <CL command to be run>\n",argv[0]);
		return (-1);
	}
	for (idx = 0;idx<argc;idx++){
		printf("argv[%d]=%s\n",idx,argv[idx]);
	}
	rtn = systemCL(argv[1],0);

	printf("rtn=%d,errno=%d\n",rtn,errno);
}
