#include <unistd.h>
#include <stdio.h>
#include <sys/systemcfg.h>
int main(){
	/*int count = sysconf(_SC_NPROCESSORS_CONF);
	printf("CPU count=%d\n",count);*/
	printf("system_configure.ncpus=%d\n",_system_configuration.ncpus);
}
