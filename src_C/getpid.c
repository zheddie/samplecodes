#include <stdio.h>
#include <stdlib.h>
extern pid_t _getpid();
int main(int argc, char *argv[])
{ 
	if(1 == argc) {
		printf("Please input a group id\n");
		exit(0);
	}

	pid_t pid = _getpid();
	printf("Done.pid=%d\n",pid);
	return(0);
}
