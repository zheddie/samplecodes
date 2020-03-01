#include <stdio.h>
#include <math.h>
#include <recio.h>
#include <iconv.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
 
 
/*#pragma mapinc("datafile","GAVIN/PASEDATA(*all)","both",,,"")
#include "datafile"
#pragma argument(ileProcedure, nowiden) 
not necessary */
 
/*
 * The arguments and function result for this ILE procedure
 * must be equivalent to the values presented to _ILECALL
 * function in the i5/OS program
 */
int dividzero(int s)
{
	printf("in ILE, before double\n");
	sleep(20);
	double x=4.0;
	x = x/0.0;
	printf("in ILE , ater double\n");
	sleep(20);
	printf("zg.inILEProcedure,x=%f\n",x);
	return 0;
}
