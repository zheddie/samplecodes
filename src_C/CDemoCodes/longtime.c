#include <stdio.h>

#define XXX 666
typedef int zint;
int main(){
int a,b;
int pid = getpid();
zint s=8;
printf("PID=%d,s=%d\n",pid,s);
for(a=0;a<10;a++){
	printf("HelloCore!a=%d,xxx=%d\n",a,XXX);
	sleep(1);
}
return(0);
}
