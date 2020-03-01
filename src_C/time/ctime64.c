#include <stdio.h>
#include <string.h>
#include <time.h>
int main(){
	time_t ticks = 0;
	ticks = time(NULL);
	ticks+=1101786527;
	printf("sizeof(p):%ld,ticks:%ld,ctime:%s\n",sizeof(int *),ticks,ctime(&ticks));
	time64_t ticks64 = 0;
	ticks64 = time64(NULL);
	ticks64+=1101786527;
	printf("sizeof(p):%ld,ticks:%lld,ctime:%s\n",sizeof(int *),ticks64,ctime64(&ticks64));
}
