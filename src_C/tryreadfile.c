#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

int main(){
	char * filename = "/QOpenSys/home/zhanggan/package.json";
	int rt = open("/QOpenSys/home/zhanggan/package.json",O_RDWR);
	int f = dup(2);
	close(f);	
	close(2);
	if(0 != rt){
		fprintf(stdout,"zg.out.rt=%d,errno=%d\n",rt,errno);
		fprintf(stderr,"zg.err.rt=%d,errno=%d\n",rt,errno);
	}else{
		fprintf(stdout,"zg.out.rt=%d,errno=%d\n",rt,errno);
		fprintf(stderr,"zg.err.rt=%d,errno=%d\n",rt,errno);
		close(rt);
	}
	printf("zg.getchar()\n");
	getchar();
	return(0);
}
