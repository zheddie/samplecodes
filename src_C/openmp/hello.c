#include <omp.h>
#include <stdio.h>
#include <unistd.h> // sleep
#include <sys/types.h>
#include <sys/syscall.h>


void sleep_sort(int *data,int size) {

    pid_t tid= 0;
    unsigned cpu,node;
    int tn,tc;

    #pragma omp parallel num_threads(size) private(tid,cpu,node)
    {
	tn = opm_get_thread_num();
	tc = opm_get_num_threads();
        #pragma omp for
        for(unsigned long i = 0; i < size; i++) {
            //sleep(data[i]);
            for(int t = 0;t<data[i] ;t++){
	    	tid=syscall(SYS_gettid);
	    	syscall(SYS_getcpu,&cpu,&node,NULL);
            	printf("data[%d]=%llu,tid=%d,cpu=%d,node=%d,thisthread=%d,threadcount=%d\n",i, data[i],tid,cpu,node,tn,tc);
		}
        }
    }
}

int main(int argc, char *argv[]) {

    int data[argc-1];
    #pragma omp parallel for
    for(int i = 0; i < argc-1; i++) {
        sscanf(argv[i+1], "%d", &data[i]);
    }
    sleep_sort(data,argc-1);

    return 0;
}
