#include <omp.h>
#include <iostream>
#include <unistd.h>
#include <sys/syscall.h>

int main(){
int cpu = 0;
#pragma omp parallel num_threads(5)
#pragma omp parallel for private(cpu)
for (int i=0;i<1000;i++){
	syscall(SYS_getcpu,&cpu,NULL,NULL);
	std::cout<<"i="<<i<<",thread:"<<omp_get_thread_num()<<",cpu="<<cpu<<std::endl;
}
return(0);
}
