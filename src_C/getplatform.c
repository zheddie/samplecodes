#include <stdio.h>
#ifndef PLATFORM
#define PLATFORM "unknown"
#endif
int main(){
	printf("platform=%s\n",PLATFORM);
}
