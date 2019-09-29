#include <stdio.h>
#include <syslog.h>

int main(){
	printf("Trying to throw message to syslog...\n");
	syslog(LOG_ALERT,"Error 99 is got.");
	return(0);

}
