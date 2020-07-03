#include <stdio.h>
#include <unistd.h>
#include <procinfo.h>
#include <sys/types.h>

int main(){
	struct procentry64 pe;
	printf("HelloGetProcs!\n");
	pid_t cld=0;
	int rt = 0;
	if(cld = fork()){
		printf("parent:%d,cld_pid:%d\n",getpid(),cld);
		rt = getprocs64(&pe, sizeof(pe), NULL, 0, &cld, 1);
		printf("parent:rt=%d\n",rt);
	}
	else{/*child process*/
		printf("child:mypid:%d\n",getpid());
		rt = getprocs64(&pe, sizeof(pe), NULL, 0, &cld, 1);
		printf("child:rt=%d\n",rt);
	}
}
