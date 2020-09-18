#include <stdio.h>
#include <sys/time.h>
#typedef void (*gettimeofday)(int);

int main(int argc, char *argv[])
{
	gettimeofday z = NULL;
	printf("zg.getTime=%d\n",z);
}
