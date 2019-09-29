#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0,idx = 0,n=0,on=1;
    struct sockaddr_in serv_addr;
    char recvBuff[1024];
//    char sendBuff[1025];
    time_t ticks;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
//    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

//    int rc = ioctl(listenfd, FIONBIO, (char *)&on);
//    if (rc < 0)
//    {
//      perror("ERROR:ioctl() failed");
//      close(listenfd);
//      exit(-1);
//    }
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);

    while(1)
    {
    	idx++;
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        printf("zg.idx=%d,listenfd=%d,connfd=%d\n",idx,listenfd,connfd);
//        ticks = time(NULL);
//        snprintf(sendBuff, sizeof(sendBuff), "Msg from server:%.24s\r\n", ctime(&ticks));
//        write(connfd, sendBuff, strlen(sendBuff));

    	memset(recvBuff, '0', sizeof(recvBuff));

    	while ((n = read(connfd, recvBuff, sizeof(recvBuff) - 1)) > 0) {
    		recvBuff[n] = 0;
    		if (fputs(recvBuff, stdout) == EOF) {
    			printf("ERROR : fputs error,errno=%d\n",errno);
    		}
    	}
    	if(0 > n){
    		printf("ERROR : read error,readreturn=%d,errno=%d\n",n,errno);
    	}
        close(connfd);
        /*sleep(1);*/
     }
}
