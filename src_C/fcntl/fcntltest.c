#include <stdio.h>
#include <fcntl.h>
int main(){
	char *filename = "./fcntltest.c";
	int fd =  open(filename,O_RDWR);
	char fullName[100];
	fullName[0]='\0';
	fcntl(fd, F_GETPATH, fullName);
	printf("fullName=%s\n",fullName);
}
