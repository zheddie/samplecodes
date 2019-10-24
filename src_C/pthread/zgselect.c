#include <stdio.h>
#include <stdlib.h>
int main(){
	struct timeval t;
	t.tv_sec = 0;
	t.tv_usec = 100;
	printf("zg.before select\n");
	select(NULL,NULL,NULL,NULL,&t);
	printf("zg.end select\n");
}
