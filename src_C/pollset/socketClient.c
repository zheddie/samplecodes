#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

typedef struct {
	int port;
	int timeout;
	char *pThreadName;
	char *pMsg;
}clientPara;

typedef void * (*pSocketConnect)(void *pClientPara);

int checkPthreadResult(char *pStr, int rc) {

	if ((pStr != NULL) && (rc!=0) ) {
		printf("client: %s failed, return value: %d, errno: %d\r\n", pStr, rc, errno);
		return -1;
	}

	return 0;
}

int createAndJoinThread(pSocketConnect pSocketCreatAndConnect, void *pClientPara) {

   pthread_attr_t threadAttribute;
   pthread_t tid;

   void *pRet;

   int rc = pthread_attr_init(&threadAttribute);
   if ( 0 != checkPthreadResult("pthread_attr_init", rc)) {
	   return -1;
   }

   rc = pthread_create( &tid, NULL, pSocketCreatAndConnect, (void *)pClientPara );
   if ( 0 != checkPthreadResult("pthread_create", rc)) {
   	   return -1;
   }

   /*
   rc = pthread_join( tid, &pRet);
   if ( 0 != checkPthreadResult("pthread_join", rc)) {
       return -1;
   }
   */

   return rc;
}

void * createConnSocketAndSendRecData (void *pParameter) {

	char receiveBuf[1024] = {0};
	int receiveBytes = 0;
	int sendBytes = 0;
	clientPara *pClientPara = (clientPara *)pParameter;

	struct sockaddr_in serverAddr;

	if (pClientPara == NULL) {
        printf("Client createConnSocketAndSendRecData: paramter is not correct.\r\n");
        return NULL;
	}

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	//inet_aton("127.0.0.1", (struct in_addr *)&serverAddr.sin_addr);
	serverAddr.sin_port = htons(pClientPara->port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	int sockFd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockFd < 0) {
        printf("Client createConnSocketAndSendRecData: create socket failed for %s. sockFd: %d, errno: %d\r\n", pClientPara->pThreadName, sockFd, errno);
        return NULL;
	}

	int iRet = connect(sockFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if ( iRet < 0) {
        printf("Client createConnSocketAndSendRecData: connect socket failed for %s. iRet: %d, errno: %d\r\n", pClientPara->pThreadName, iRet, errno);
        return NULL ;
	}

    //send data to server:
	sendBytes = send(sockFd, pClientPara->pMsg, strlen(pClientPara->pMsg)+1, 0);
	if (sendBytes < 0) {
	    printf("Client createConnSocketAndSendRecData: send data failed for %s. sendBytes: %d, errno: %d\r\n", pClientPara->pThreadName, sendBytes, errno);
        return NULL;
	}

	//receive data from server, and print these data:
    int i = 0;
    while(i<5) {
    	receiveBytes = recv(sockFd, receiveBuf, sizeof(receiveBuf)-1, 0);
    	if (receiveBytes < 0) {
    	    printf("Client createConnSocketAndSendRecData: recv data failed for %s. receiveBytes: %d, errno: %d. times: %d\r\n", pClientPara->pThreadName, receiveBytes, errno, i);
    	    sleep(pClientPara->timeout);
    	    continue;
    	} else {
    		printf("Client %s received data from server: %s. times: %d\r\n", pClientPara->pThreadName, receiveBuf, i);
    		break;
    	}
    }
    free(pClientPara->pThreadName);
    free(pClientPara->pMsg);
    close(sockFd);

    return NULL;
}




void main(int argc, char *argv[]) {

    int threadNum = 0;
    int port = 0;

    if (argc <= 2) {
    	threadNum = 5;
    	port = 8010;
    } else {
    	threadNum = atoi(argv[1]);
    	port = atoi(argv[2]);
    	printf("Client main: thread number is: %d, port: %d\r\n", threadNum, port);
    }

    int i = 0;
    for (i = 0; i < threadNum; i++) {

    	//clientPara sClientPara;
    	//memset(&sClientPara, 0, sizeof(sClientPara));

    	clientPara *pClientPara =(clientPara *) malloc(sizeof(clientPara));
    	if (pClientPara == NULL) {
            printf("Client main: malloc for clientPara failed\r\n");
            return;
    	}

    	pClientPara->port = port;
    	pClientPara->timeout = 1;
    	pClientPara->pMsg = (char *)malloc(1024);
    	if (pClientPara->pMsg == NULL) {
    		printf("Client main: malloc for pMsg failed\r\n");
    		return;
    	}
    	memset(pClientPara->pMsg, 0, 1024);

    	pClientPara->pThreadName = (char *)malloc(64);
    	if (pClientPara->pThreadName == NULL) {
    	    printf("Client main: malloc for pThreadName failed\r\n");
    	    return;
    	}
    	memset(pClientPara->pThreadName, 0, 64);

    	snprintf(pClientPara->pThreadName, 64-1, "%s%s%d", "client", "-", i);
    	//printf("Debug Client: sClientPara.threadName: %s\r\n", sClientPara.threadName);
    	snprintf(pClientPara->pMsg, 1024-1, "%s%s", "hello server from ", pClientPara->pThreadName);

    	if (createAndJoinThread(createConnSocketAndSendRecData, pClientPara) < 0 ) {
    		printf("Client main: createAndJoinThread failed. client name: %s\r\n", pClientPara->pThreadName);
    		continue;
    	}
    }
    sleep(5);

}
