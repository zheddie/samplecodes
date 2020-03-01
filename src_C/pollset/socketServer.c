
#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

typedef struct {
	int sockFd;
	char *pMsgFromClient;
	char *pMsgToClient;
} serverPara;

typedef void (*pPrintReceivedInfo)(void *data);

void printReceivedInfo(void *pData) {

	serverPara *pServerPara = (serverPara *)pData;

	if ((NULL != pServerPara) && (pServerPara->pMsgFromClient != NULL)) {
		printf("server received request: %s\r\n", pServerPara->pMsgFromClient);
	}

	int sendBytes = send(pServerPara->sockFd, pServerPara->pMsgToClient, strlen(pServerPara->pMsgToClient)+1, 0);
	if (sendBytes < 0) {
	    printf("Server main: send data failed. sendBytes: %d, errno: %d\r\n", sendBytes, errno);
	}

	free(pServerPara->pMsgFromClient);
	free(pServerPara->pMsgToClient);
	close(pServerPara->sockFd);

	free(pServerPara);
}

int checkPthreadResult(char *pStr, int rc) {

	if ((pStr != NULL) && (rc!=0) ) {
		printf("server: %s failed, return value: %d\r\n", pStr, rc);
		return -1;
	}

	return 0;
}

int createAndJoinThread(pPrintReceivedInfo pPrintInfo, void *pPrintMsg) {

   pthread_attr_t threadAttribute;

   pthread_t threadId;

   void *pRet;

   int rc = pthread_attr_init(&threadAttribute);
   if ( 0 != checkPthreadResult("pthread_attr_init", rc)) {
	   return -1;
   }

   rc = pthread_create( &threadId, NULL, pPrintInfo, (void *)pPrintMsg );
   if ( 0 != checkPthreadResult("pthread_create", rc)) {
   	   return -1;
   }

   /*
   rc = pthread_join( threadId, &pRet);
   if ( 0 != checkPthreadResult("pthread_join", rc)) {
      	   return -1;
   }
   */

   return rc;
}

int createBindAndListenSocket(int port, int backLog) {

	struct sockaddr_in hostAddr;

	memset(&hostAddr, 0, sizeof(hostAddr));
	hostAddr.sin_family = AF_INET;
	hostAddr.sin_port = htons(port);
	hostAddr.sin_addr.s_addr = INADDR_ANY;

	int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket < 0 ) {
		printf("createBindAndListenSocket: create socket failed. listenSocket: %d, errno: %d\r\n", listenSocket, errno);
		return -1;
	}
	int iRet = bind(listenSocket, (struct socketaddr *)&hostAddr, sizeof(hostAddr));
	if (iRet < 0) {
		printf("creatBindAndListenSocket: socket bind failed. iRet: %d, errno: %d\r\n", iRet, errno);
		return -1;
	}

	iRet = listen(listenSocket, backLog);
	if (iRet < 0) {
			printf("creatBindAndListenSocket: socket listen failed. iRet: %d, errno: %d\r\n", iRet, errno);
			return -1;
    }

	return listenSocket;
}


void main (int argc, char *argv[]) {

	int backLog = 0;
	int port = 0;

	int acceptSocket = 0;
	struct sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);
	memset(&clientAddr, 0, sizeof(clientAddr));

	char receiveBuf[1024];

	if (argc <= 2) {
		backLog = 5;
		port = 8010;
	} else {
		backLog = atoi(argv[1]);
		port = atoi(argv[2]);
		printf("Server main: backLog value is: %d, port: %d\r\n", backLog, port);
	}

	printf("Server main: start\r\n");
	int listenSocket = createBindAndListenSocket(port, backLog);

	while(1) {
		memset(receiveBuf, 0, sizeof(receiveBuf));

		acceptSocket = accept(listenSocket, (struct socketaddr *)&clientAddr, &clientAddrLen);
        if (acceptSocket < 0) {
        	printf("Server main: accept socket failed. acceptSocket: %d, errno: %d\r\n", acceptSocket, errno);
        	return;
        }

        //receive data:
        int receiveBytes = recv(acceptSocket, receiveBuf, sizeof(receiveBuf)-1, 0);
        if (receiveBytes < 0) {
        	printf("Server main: recv data failed. receiveBytes: %d, errno: %d\r\n", receiveBytes, errno);
        	continue;
        }



        serverPara *pServerPara = malloc(sizeof(serverPara));
        if (pServerPara == NULL) {
        	printf("Server main: malloc for serverPara failed\r\n");
        	return;
        }
        pServerPara->pMsgFromClient = malloc(1024);
        if (pServerPara->pMsgFromClient == NULL) {
			printf("Server main: malloc for pMsgFromClient failed\r\n");
			return;
		}

        pServerPara->pMsgToClient = malloc(1024);
		if (pServerPara->pMsgToClient == NULL) {
			printf("Server main: malloc for pMsgToClient failed\r\n");
			return;
		}

		pServerPara->sockFd = acceptSocket;
		snprintf(pServerPara->pMsgFromClient, 1024-1, "%s%d%s%s", "received message from socket ", acceptSocket, ": ", receiveBuf);
		snprintf(pServerPara->pMsgToClient, 1024-1, "%s%s", "Reply for the message: ", receiveBuf);


        //create a thread to print the reveived message:
        createAndJoinThread(printReceivedInfo, pServerPara);

        //send data to client:
/*
        int sendBytes = send(acceptSocket, sendBuf, strlen(sendBuf)+1, 0);
        if (sendBytes < 0) {
            printf("Server main: send data failed. sendBytes: %d, errno: %d\r\n", sendBytes, errno);
            continue;
        }
        close(acceptSocket);
*/
	}
	close(listenSocket);

    printf("Server main: end\r\n");
    return;
}
