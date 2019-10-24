#include <stdio.h>
#include <net/if.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

int main(int argc,char **argv)
{
	char p[IFNAMSIZ];
	memset(p,0,IFNAMSIZ);
	char *rc = NULL;
	int index = 0;
	if(argc >1) index = atoi(argv[1]);
	else return -1;
	printf("argv[1]=%s,index=%d\n",argv[1],index);
	rc = if_indextoname(index,p);
	if (rc == NULL)
	{
		printf("if_indextoname() failed with errno =  %d %s \n",
				errno,strerror(errno));
		return -1;
	}
	printf("name of the index(%d) is :%s\n",index,p);
}
