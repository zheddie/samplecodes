#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef VERIFY
#define VERIFY 0
#endif

int randomnumber(int b)
{
    return rand()%(b+1);
}


int main(){
	float tmp = 0.;
	int i=0;
	clock_t clocktime = 0;
	unsigned long long timeconsumed1 = 0,timeconsumed2 = 0;

	clocktime = clock();
	for(i=0;i<10000*10000;i++){
		tmp+=1.0;
	}
	timeconsumed1 = clock()-clocktime;
	printf("Timeconsumed: %llu/%d,tmp=%.2f\n",timeconsumed1,CLOCKS_PER_SEC,tmp);
}

