#define _MULTI_THREADED
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <qp2user.h>
//#include "check.h"


int main(int argc, char **argv)
{
	/*try to call pase program which did same thing as below, we expect that this would not crash*/
	char pathname[]="/home/zhanggan/dvdzero";
	char ** argv = NULL;
	argvv = (char **)malloc(sizeof(char *)*2);
	argvv[1]=(char*) malloc(sizeof(pathname)+1);
	argvv[2] = NULL;
	strcpy(argvv[1],pathname);
	int rt = Qp2RunPase(pathname,NULL,NULL,0,819,argvv,NULL);
	printf("rt=%d\n",rt);
	int r=1;
	int izero = 0;

	double zero = 0.0;
	double average = 1;
	double zz = +0.0;
	/*  r = r/izero;
	    printf("r=%d\n",r);
	    */
	average = average/zero;
	printf("average=%f\n",average);
	zz = zz/zero;
	printf("zz=%f\n",zz);
	return 0;
}
