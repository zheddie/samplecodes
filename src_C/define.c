#include <stdio.h>
#define PRINT(name) "/usr/bin/"#name
int main(){
	printf("hello %s\n",PRINT(hhhh));
	printf("hello %s\n",PRINT(iiiii));
	return(0);
}
