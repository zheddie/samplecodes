#include <stdio.h>
#include <fcntl.h>
int main(){
	char line[10];
	int fp = open("a.txt",O_RDONLY|O_NOFOLLOW);
	int rc = read(fp,line,10);
	printf("fp=%d,rc=%d\n",fp,rc);
	if(rc>0){write(1,line,rc);}
	close(fp);
}
