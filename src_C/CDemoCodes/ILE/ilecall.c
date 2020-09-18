#define _MULTI_THREADED
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <qp2user.h>
#include <qp2shell2.h>
//#include "check.h"


int main(int argc, char **argv)
{
	/*try to call pase program which did same thing as below, we expect that this would not crash*/
	char pathname[23];
	memset(pathname,0,23);
	strcpy(pathname ,"/home/zhanggan/dvdzero");
	QP2SHELL2(pathname,NULL);
	printf("Return from PASE1\n");
	QP2SHELL2(pathname,NULL);
	printf("Return from PASE2\n");

	return 0;
}
