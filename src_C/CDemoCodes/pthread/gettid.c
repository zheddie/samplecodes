#include <stdio.h>
#include <pthread.h>

int main(){
	pthread_id_np_t   tid;
	tid = pthread_getthreadid_np();
	printf("threadID:%d\n",tid);
}
