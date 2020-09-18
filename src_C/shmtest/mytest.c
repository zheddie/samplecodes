#include <stdio.h>
#include <sys/shm.h>
#include <errno.h>
int main(int argc,char * argv[]){
	int size = 10;
	if(argc >1){
		size = atoi(argv[1]);
	}
	void * currentAddress =(void *) 0x0700000000000000;
	// void * currentAddress =(void *) 0;
	// unsigned long S5G = 0x140000000;
	unsigned long S1G = 0x40000000;
	// unsigned long S10G = 0x280000000;
	// unsigned long S100G = 0x1900000000;

	unsigned long byteAmount = S1G * size;
	void *memoryPointer = NULL;
	key_t addressKey;
	int shmgetFlags = IPC_CREAT;
	shmgetFlags |= S_IRUSR;
	shmgetFlags |= S_IWUSR;
	printf("cp0,shmgetFlags=%x,byteAmount=%lu\n",shmgetFlags,byteAmount);
	addressKey = shmget(IPC_PRIVATE, (size_t)byteAmount, shmgetFlags);
	if (-1 == addressKey) {
		printf("shmget failed!\n");
		exit(-1);
	} else {
		struct shmid_ds mutate;

		memset(&mutate, 0x0, sizeof(struct shmid_ds));
		mutate.shm_pagesize = 64*1024;

		if (-1 == shmctl(addressKey, SHM_PAGESIZE, (struct shmid_ds *)&mutate)) {
			printf("shmctl() failed\n");
			exit(-2);
		}
		printf("cp1,addressKey=%d,currentAddress=%p\n",(int)addressKey,currentAddress);
		memoryPointer = shmat(addressKey,  currentAddress, 0);
		if(-1 == (long)memoryPointer){
			printf("shmat() failed. errno=%d\n",errno);
			exit(-4);
		}
		printf("cp2.memoryPointer=%p\n",memoryPointer);
		if (0 == shmctl(addressKey, IPC_RMID, NULL)) {
			if (NULL != memoryPointer) {
				printf("cp3.memoryPointer=%p,byteAmount=%lu\n",memoryPointer,byteAmount);
				if(0==mprotect(memoryPointer, byteAmount,0x3)){
					printf("mprotect() failed\n");
					exit(-3);
				}
			}
		} else {
			printf("shmctl() failed\n");
			/* if releasing addressKey fails detach memory to avoid leaks and fail */
			if (NULL != memoryPointer) {
				if (0 != shmdt(memoryPointer)) {
					printf("Detach failed!\n");
				}

				memoryPointer = NULL;
			}
		}
	}
	if(memoryPointer!=NULL){
		void * ptr =  memoryPointer;
		for(int x =0 ;x<size;x++){
			memset(ptr,0,S1G);
			sleep(1);
			printf("%d:ADDR: %p\n",x,ptr);
			ptr = (void *)((unsigned long )ptr+S1G);
			//printf("%d:next ADDR: %p\n",x,ptr);
		}
	}	
	printf("zg.cp9.memoryPointer=%p\n",memoryPointer);

	return(0);


}