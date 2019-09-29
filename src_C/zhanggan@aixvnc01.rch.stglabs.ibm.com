#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/ioctl.h>


#define NUMTHREADS 400

typedef struct _Parm {
  int thdid;
}Parm;

char HostName[16];

void * thread_func(void *param) {
	int n = 0;
//	char recvBuff[1024];
	char sendBuff[1024];
	struct sockaddr_in serv_addr;
	Parm * p = (Parm *)param;
	time_t ticks;
	int sockfd = 0 ;
	int off = 0,on=1;
	long rd = 0;

	srand(time(0));
	rd = rand();
	int slp = rd % 10;
	printf("starting thread:%d after %d seconds\n",p->thdid,slp);
	//sleep(slp);
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(5000);

	if (inet_pton(AF_INET, HostName, &serv_addr.sin_addr) <= 0) {
		printf("%d:inet_pton ERROR occured\n",p->thdid);
		goto rtn_line;
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n ERROR : Could not create socket \n");
		goto rtn_line;
	}
//    int rc = ioctl(sockfd, FIONBIO, (char *)&off);
//    if (rc < 0)
//    {
//      perror("ERROR:ioctl() failed");
//      close(sockfd);
//      goto rtn_line;
//    }
	printf("%d:sockfd=%d\n",p->thdid,sockfd);
	int conrt = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	printf("%d,connect return:%d\n",p->thdid,conrt);
	if (conrt< 0) {
		printf("%d:ERROR : Connect Failed ,conrt=%d,errno=%d\n",p->thdid,conrt,errno);
		goto rtn_line;
	}

    ticks = time(NULL);
    snprintf(sendBuff, sizeof(sendBuff), "Msg to server:%.24s:%d\r\n", ctime(&ticks),p->thdid);
    //n = write(sockfd, sendBuff, strlen(sendBuff));
    n = send(sockfd, sendBuff, strlen(sendBuff),0);
    if(n != strlen(sendBuff)){
    	printf("%d:Write ERROR,tobesend:%lu,sent:%d,errno=%d \n",p->thdid,sizeof(sendBuff),n,errno);
    }else{
    	printf("%d:MSG sent:%s\n",p->thdid,sendBuff);
    }
    memset(sendBuff,0,strlen(sendBuff));
    n  = 0;
    n = recv(sockfd,sendBuff,sizeof(sendBuff),0);
//    while (n <=0){
//    	n = recv(sockfd,sendBuff,strlen(sendBuff),0);
//    	sleep(1);
//    }
//    printf("%d:Received from Server:%s\n",p->thdid,sendBuff);
    if( n > 0){
    	printf("%d:Received from Server:%s\n",p->thdid,sendBuff);
    }else if(n == 0){
    	printf("%d:Connection closed?\n",p->thdid);
    }else{
    	printf("%d:ERROR:Nothing received,errno=%d?\n",p->thdid,errno);
    }

//
//	memset(recvBuff, '0', sizeof(recvBuff));
//
//	while ((n = read(sockfd, recvBuff, sizeof(recvBuff) - 1)) > 0) {
//		recvBuff[n] = 0;
//		sprintf(&recvBuff[n-2],":%d\r\n",p->thdid);
//		if (fputs(recvBuff, stdout) == EOF) {
//			printf("%d:ERROR : Fputs ERROR\n",p->thdid);
//		}
//	}

//	if (n < 0) {
//		printf("%d:Write ERROR,errno=%d \n",p->thdid,errno);
//	}

rtn_line:
	close(sockfd);
	printf("%d:exited.\n",p->thdid);
	return NULL;
}

int main(int argc, char *argv[]) {
	int i = 0,maxthd=0;
	Parm * pm = NULL;

	pthread_t * thread;
	if(argc < 2){
		printf("Usage:\n\t%s <threadNumber> <ipaddress>\n",argv[0]);
		return(-1);
	}
	maxthd  = atoi(argv[1]);
	if(maxthd == 0){
		maxthd = NUMTHREADS;
	}

	thread = (pthread_t *)malloc(sizeof(pthread_t)*maxthd);
	memset(HostName,0,16);
	if(argc == 3){
		strcpy(HostName,argv[2]);
	}else{
		strcpy(HostName,"127.0.0.1");
	}


	for (i = 0; i < maxthd; ++i) {
		// start threads and give each a own logfile and loglevel
//      char file[200];
		pm = (Parm *)malloc(sizeof(Parm));
		memset(pm,0,sizeof(Parm));

		pm->thdid = i;
		pthread_create(&thread[i], NULL, thread_func, pm);
	}
	for (i = 0; i < maxthd; ++i) {
		pthread_join(thread[i], NULL);
	}

	return 0;
}
