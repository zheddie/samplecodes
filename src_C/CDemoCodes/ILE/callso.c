#include <stdio.h>
#include <unistd.h>
#include <string.h>

extern "C"{
	int dividzero();
}



int main(int argc, char **argv)
{
	int rt = dividzero();
	return rt;
}
