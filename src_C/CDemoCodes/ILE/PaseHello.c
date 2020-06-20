/* Name: PASEtoILE.c  
 *  
 * You must use compiler options -qalign=natural and -qldbl128  
 * to force relative 16-byte alignment of type long double  
 * (used inside type ILEpointer)  
 *
 */
#include <stdlib.h>
#include <stdio.h>
 

void main (int argc , char *argv[]) {
	int secs= 30,index=0;
	if(argc >1){
		secs = atoi(argv[1]);
	}
	printf("HelloPASE.In sleeping\n");
	for(index = 0;index<secs;index++){
		sleep(1);
		printf(".");
		fflush(NULL);
	}
      printf("\nHelloPASE.Exiting sleep\n");
}
