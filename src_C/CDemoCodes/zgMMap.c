#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
int main(int argc , char ** argv){
	unsigned int startA = 0x1000;
	unsigned int endA = 0xefffffff;
	unsigned int p = 0;
	int iCount = 4;
	int iM = 300;
	if(argc>1){
		iCount = atoi(argv[1]);
	}

	if(argc>2){
		iM = atoi(argv[2]);
	}
	printf("sizeof(int)=%d,sizeof(long)=%d\n",sizeof(int),sizeof(long));
	unsigned long byteAmount = iM*1024*1024;
	for(int j = 0;j<iCount ;j++){
		int wCount = 0;
		void * result = NULL;
		void * rm = NULL;
		p = startA;
		while (p<=endA){
			result = mmap((void *)p,byteAmount,3,273,-1,0);
			//result = mmap((void *)p,byteAmount,3,274,-1,0);

			if(result != MAP_FAILED){
//				errno=0;
//				rm = mmap((void *)p,byteAmount,3,273,-1,0);
//				int zerr  = errno;
//				if(rm == MAP_FAILED){printf("ERROR!!!Can not overlay on result=%p,p=%p,errno=%d\n",result,(void *)p,zerr);}
//				if(rm != result){printf("ERROR!!!Can not overlay on result=%p,p=%p,rm=%p\n",result,(void *)p,rm);}
				//memset(result,0,1);
				break;}
	#ifdef LINUX
			if(errno != 9) {printf("errno=%d,p=%p\n",errno,(void *)p);}
	#endif
	#ifdef AIX
			if(errno != 12) {printf("errno=%d,p=%p\n",errno,(void *)p);}
	#endif
			p += 0x1000;
			wCount ++;
		}
		p -= 0x1000;
		printf("wCount = %d,result=%p,p=%p,index=%d\n",wCount,result,(void *)p,j);
		if(result != NULL){
			munmap(result,byteAmount);
		}

	}
}
