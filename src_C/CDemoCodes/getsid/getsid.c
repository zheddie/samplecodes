#include <stdio.h>
#include <unistd.h>

int main() {
	pid_t pid, gid, sid;
	pid = getpid();
	gid = getpgid(0);
	sid = getsid(0);
	printf("In Parent1:pid=%d,gid=%d,sid=%d\n",pid,gid,sid);
	pid_t cpid = fork();
	if (cpid == 0) { /*Child*/
		pid = getpid();
		gid = getpgid(0);
		sid = getsid(0);
		printf("In Child:pid=%d,gid=%d,sid=%d\n",pid,gid,sid);

	} else {/*parent*/
		pid = getpid();
		gid = getpgid(cpid);
		sid = getsid(cpid);
		printf("In Parent2:pid=%d,gid=%d,sid=%d\n",pid,gid,sid);

	}
}
