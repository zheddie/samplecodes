#include <procinfo.h>   //getprocs
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#define CHILDLIMIT 10
#define BUFFSIZE 100
int main()
{
	
    
    struct procsinfo * ProcessBuffer;
	
	pid_t parentpid=getpid();  //parent pid
	//pid_t pid = 0;
	pid_t *pidx = (pid_t *)malloc(sizeof(pid_t));
	pid_t *allchilds = NULL;
	allchilds = (pid_t *)malloc(sizeof(pid_t)*CHILDLIMIT);
	memset(allchilds,0,sizeof(pid_t)*CHILDLIMIT);
	*pidx = 0;
	int ret = 0;

	int childCount=0;
	ProcessBuffer = (struct procsinfo *)malloc(sizeof(struct procsinfo )* BUFFSIZE);
	int cpid = 0;
	for (int x = 0;x<CHILDLIMIT;x++){
		cpid = fork();
		if( cpid == 0){	
	  		//child process
	  		printf("zg.inchild.pid:%d.sleeping...%d...\n",getpid(),x);
			sleep(x);
			printf("zg.inchild.pid:%d.END\n",getpid());
			return(0);
		}
		else{
			allchilds[x] = cpid;
		}
	}

	printf("in parent pid:%d\n", parentpid);
	sleep(3);
	while(1)
	{

		ret = getprocs(ProcessBuffer,  sizeof(struct procsinfo), NULL, 0, pidx, BUFFSIZE);
		//printf("zg.pidx=%d,ret=%d\n",*pidx,ret);
		if( ret == -1)
		{
			printf("The ret is :%d,errno=%d\n",ret,errno);
			return -1;
		}
	
		for(int y = 0;y<ret;y++){
			if ( ProcessBuffer[y].pi_ppid == parentpid) 
			{
				printf("Retrieved child pid is[%d]:%d,cwd:%d,vmm:%ld\n",y, ProcessBuffer[y].pi_pid,ProcessBuffer[y].pi_cdir,ProcessBuffer[y].pi_dvm);
				
				//printf("Retrieved child pid is:%d,cwd:%s,vmm:%d\n", ProcessBuffer[y].pi_pid,ProcessBuffer[y].pi_cdir,ProcessBuffer[y].pi_dvm);

				childCount++;
				
			}
		
		}
		if(ret < BUFFSIZE){ //No more entries in the table.
			printf("Total child got:%d\n",childCount);
			return (0);
		}
	}//whild
	
	return 0;
}
