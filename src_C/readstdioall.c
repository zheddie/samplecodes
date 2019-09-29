#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/errno.h>
#include <sys/pollset.h>
#include <time.h>
#define NOMSGQS		0
#define	MSGQS		1
extern int _select(uint nmsgs, void *rlist, void *wlist, void *elist, struct timeval *utp, int msgqsupport);
int 
pselect(int nfds, void *rlist, void *wlist, void *elist, const struct timespec *timeout, const sigset_t *sigmask)
{
	int rc, save_errno;
	sigset_t  oset;
	struct timeval utp, *tv = (struct timeval *)0;
	
	save_errno = errno;
	if(sigmask)
		if(sigprocmask(SIG_SETMASK, sigmask, &oset) < 0)
			return -1;

	if(timeout)
	{
		utp.tv_sec = timeout->tv_sec;
		utp.tv_usec = timeout->tv_nsec / 1000;
		tv = &utp;
	}
	
	while(1) {
		rc = _select(nfds, rlist, wlist, elist, tv, NOMSGQS);

		if (rc != -1)
			break;
		if (errno != ERESTART)
			break;
		else
			errno = save_errno;
	}

	if(sigmask)
	{
		save_errno = errno;
		sigprocmask(SIG_SETMASK, &oset, (sigset_t *)0);
		errno = save_errno;
	}
	return rc;
}
int main(){
	unsigned char c;
	int result,rt;
	sigset_t empty_set;
	fd_set readfds;
	sigemptyset(&empty_set);
	FD_ZERO(&readfds);
	FD_SET(0,&readfds);
	while(1){
	result=pselect(1,&readfds,NULL,NULL,NULL,&empty_set);
	printf("result=%d\n",result);
	if(result >=0){
		rt = read(0,&c,sizeof(unsigned char));
		if(rt == sizeof(unsigned char)) {
			printf("%c\n",c);
			if( c == 'q') return(0);
		}
	}
	}
	return(rt);
}
