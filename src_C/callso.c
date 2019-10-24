#include <stdio.h>
#include <unistd.h>
#include <string.h>

extern int dividzero();




int main(int argc, char **argv)
{
	int rt = dividzero();
	return rt;
}
