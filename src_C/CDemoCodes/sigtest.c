#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
int main(int argc, char * argv[]){
	int h = SIG_SETMASK;
	int i = 0;
	int size = 0;
	sigset_t * current, * new;
	size = sizeof(sigset_t);
/*	current = (sigset_t *)malloc(size);
	memset(current,0,size);*/
	new = (sigset_t *)malloc(size);
	memset(new,0,size);
	new->__val[9] = (unsigned long int) 8;
	printf("_SIGSET_NWORDS=%d,size=%d\n",_SIGSET_NWORDS,size);
	printf("(0)current = %p,new = %p\n",current,new);

	pthread_sigmask(h, new,current);
	printf("(1)current = %p,new = %p\n",current,new);
	for (i=0;i<_SIGSET_NWORDS;i++){
		printf("SIGMASK[%d]=%p\n",i,current->__val[i]);
	}
	pthread_sigmask(h, new,current);
	printf("(2)current = %p,new = %p\n",current,new);
	for (i=0;i<_SIGSET_NWORDS;i++){
		printf("SIGMASK[%d]=%x\n",i,current->__val[i]);
	}

}
