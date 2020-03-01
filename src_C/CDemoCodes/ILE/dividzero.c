#include <stdio.h>
#include <unistd.h>
#include <string.h>




int main(int argc, char **argv)
{
	double x=1.0;
	double zero = 0.0;
	printf("1.0/0.0=%f\n",x/zero);
	return 0;
}
