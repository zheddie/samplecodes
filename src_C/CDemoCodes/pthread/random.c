#include <stdio.h>
//#include <stdlib.h>
#include <pthread.h>
int main(){
	srand((unsigned)time(0));
	int a = random(100);
	printf("random=%d\n",a);
}
