#include <iostream>
#include <cstdio>
#include <unistd.h> // sleep
#include <sys/types.h>
#include <sys/syscall.h>
#include <cstdio>
#include <omp.h>
#include <vector>


void sleep_sort(std::vector<unsigned long long> &data) {

    const int length = data.size();
    pid_t tid= 0;
    unsigned cpu=0,node=0;
    int tn=0,tc=0;

    #pragma omp parallel num_threads(length) private(tid,cpu,node)
    {
        #pragma omp for
        for(unsigned long i = 0; i < length; i++) {
            //sleep(data[i]);
            for(int t = 0;t<data[i] ;t++){
		tn = omp_get_thread_num();
		tc = omp_get_num_threads();
	    	tid=syscall(SYS_gettid);
	    	syscall(SYS_getcpu,&cpu,&node,NULL);
            	printf("data[%d]=%llu,tid=%d,cpu=%d,node=%d,thisthread=%lld,threadcount=%lld\n",i, data[i],tid,cpu,node,tn,tc);
		sleep(1);
		}
        }
    }
}

int main(int argc, char *argv[]) {

    std::vector<unsigned long long> data(argc-1);
    pid_t pid = getpid();
    printf("pid=%d\n",pid);
    #pragma omp parallel for
    for(int i = 0; i < argc-1; i++) {
        sscanf(argv[i+1], "%llu", &data[i]);
	//std::cout<<"data:["<<i<<"]=" << data[i]<<std::endl;
    }
    for(int i = 0; i < argc-1; i++) {
	std::cout<<"data:["<<i<<"]=" << data[i]<<std::endl;
    }

    sleep_sort(data);

    return 0;
}
