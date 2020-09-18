#include <pthread.h> /* include file for pthreads - the 1st */
#include <stdio.h> /* include file for printf() */ 
#include <unistd.h> /* include file for sleep() */ 
void *Thread(void *string) { 
	int i; int o_state; /* disables cancelability */ 
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &o_state); 
	/* writes five messages */ 
	for (i=0; i<5; i++) 
		printf("%s\n", (char *)string); 
	/* restores cancelability */ 
	pthread_setcancelstate(o_state, &o_state); 
	/* writes further */ 
	while (1)
	{ 
		printf("%s\n", (char *)string); 
		sleep(1);
	}
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
	rc = pthread_create(&f_th, NULL, Thread, (void *)f_str); 
	if (rc) return -1;
/* sleeps a while 
	sleep(5); 
 requests cancelation */ 
	pthread_cancel(e_th); 
	pthread_cancel(f_th); 
/* sleeps a bit more */ 
	sleep(5); 
	pthread_exit(NULL); 
}
