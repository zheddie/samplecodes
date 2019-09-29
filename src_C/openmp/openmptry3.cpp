#include <omp.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>

int main(){
#pragma omp parallel num_threads(3)
for (int i=0;i<3;i++){
	int tna = omp_get_thread_num();
    	int tca = omp_get_num_threads();
	int tosleep = rand()%3;
	sleep(tosleep);
	std::cout<<"i="<<i<<",threadnum:"<<tna<<",threadCount="<<tca<<std::endl;
}
return(0);
}
