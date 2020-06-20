#include <pthread.h> /* include file for pthreads - the 1st */
#include <stdio.h> /* include file for printf() */ 
#include <unistd.h> /* include file for sleep() */ 
void cleaner(void *buffer){
	printf("In cleaner!buffer=%x\n",buffer);
	if(buffer != NULL){
		printf("In Cleanner!Buffer != NULL\n");
		memset(buffer,0,1000);
		free(buffer);
		
		}
}
char * myBuf = NULL;
void *Thread(void *string) { 
	myBuf = (char*)malloc(1000);
	memset(myBuf,'\0',1000);
	strcpy(myBuf, "Hello pthread leak.TH01!");
	printf("In Thread!myBuf=%x\n",myBuf);
	pthread_cleanup_push(&cleaner,myBuf);
	printf("After cleanup registry!\n");
    /*
    */
	/*pthread_cleanup_pop(0);*/
	/*pthread_exit(NULL); */
} 
void *Thread2(void *string) { 
	printf("In Thread2,myBuf=%x\n",myBuf);
	strcpy(myBuf, "Hello pthread leak.TH02!");
    /*
    */
	pthread_exit(NULL); 
} 
int main() { 
	char *e_str = "Hello!"; 
	char *f_str = "Bonjour !"; 
	pthread_t e_th;
	pthread_t f_th; 
	int rc; /* creates both threads */ 

	rc = pthread_create(&e_th, NULL, Thread, (void *)e_str); 
	if (rc) 
		return -1;
	
	sleep(1);
	printf("In main!myBuf=%x\n",myBuf);
	rc = pthread_create(&f_th, NULL, Thread2, (void *)f_str); 
	if (rc) 
		return -1; 
	sleep(1);
	printf("myBuf=%s\n",myBuf);
	pthread_exit(NULL); 
}
