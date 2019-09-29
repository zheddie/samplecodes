#include <stdio.h>
#include <unistd.h>
#include <syslog.h>

int main(int argc , char * argv[]) {
	int total = 100,idx = 0;
	if(argc > 1){
		total = atoi(argv[1])*100;
		if(total <= 0){
			total = 100;
		}
	}
 openlog("slog", LOG_PID|LOG_CONS, LOG_USER);
 for(idx = 0; idx<total;idx++){
	 syslog(LOG_INFO, "Pressure test for syslog.idx=%d... ",idx);
 }
 closelog();

 return 0;
}
