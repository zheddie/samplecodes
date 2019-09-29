#include <stdio.h>
#include <unistd.h>
#include <sys/systemcfg.h>
int main(){
	int sysconf_nprocessors_conf = (int)_system_configuration.ncpus;
	printf("zg.sysconf_nprocessors_conf = %d\n",sysconf_nprocessors_conf);
	return(0);
}
