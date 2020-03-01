#include <stdio.h>
#include <string.h>
#include <time.h>
int main(){
	time_t ticks = 0;
	ticks = time(NULL);
	ticks+=1101786527;
	printf("sizeof(p):%ld,ticks:%ld,ctime:%s\n",sizeof(int *),ticks,ctime(&ticks));

}
