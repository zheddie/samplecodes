#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/select.h>
int main(){
	unsigned char c;
	int result,rt;
	sigset_t empty_set;
	fd_set readfds;
	sigemptyset(&empty_set);
	FD_ZERO(&readfds);
	FD_SET(0,&readfds);
	while(1){
	result=pselect(1,&readfds,NULL,NULL,NULL,&empty_set);
	printf("result=%d\n",result);
	if(result >=0){
		rt = read(0,&c,sizeof(unsigned char));
		if(rt == sizeof(unsigned char)) {
			printf("%c\n",c);
			if( c == 'q') return(0);
		}
	}
	}
	return(rt);
}
