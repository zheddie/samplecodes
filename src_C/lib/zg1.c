
#include <stdio.h>
#include <unistd.h>
#include <string.h>

static char str[] = "Hello from echowrite\n";

int echowrite(){
	int len = strlen(str);
#ifdef AIX
	kwrite(STDOUT_FILENO,str,len);
#else
	write(STDOUT_FILENO,str,len);
#endif
	return(0);
}
