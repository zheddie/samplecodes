
#include <stdio.h>
#include <unistd.h>

static char str[] = "Hello from echoprintf\n";

int echoprintf(){
	printf("%s",str);
	return(0);
}
