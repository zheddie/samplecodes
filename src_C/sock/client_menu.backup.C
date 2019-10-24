#include <pthread.h>
#include <stdio.h>
#include <string.h>	// memset
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_TCP_PORT   6543
#define SERV_HOST_ADDR 	"9.5.51.175"
/*#define SERV_HOST_ADDR 	"127.0.0.1"*/

typedef struct
{
  int sockfd;
} share_data_t;

void * doRecv(void * inData)
{
  char buffer[4096];
  int rc;

  /*printf("receiver thread running\n");*/
  share_data_t * data = (share_data_t *)inData;
  while(1)
  {
    int len = recv(data->sockfd, buffer, sizeof(buffer),0);
    if (len == 0)
    {
      exit(0);
    }
    else if (len == -1)
    {
      printf("error on socket\n");
    }
    else
    {
      /*printf("%s",buffer);*/
      fwrite(buffer, len, 1, stdout);
    }
  }
}

int main(void)
{
  share_data_t data;
  char achar;
  pthread_t recv_tid;
  struct sockaddr_in serv_addr;
  int rc;
  
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family		= AF_INET;
  serv_addr.sin_addr.s_addr 	= inet_addr(SERV_HOST_ADDR);
  serv_addr.sin_port		= htons(SERV_TCP_PORT);

  /*printf("socket\n");*/
  data.sockfd = socket(AF_INET, SOCK_STREAM, 0);
  assert(data.sockfd != -1);
  /*printf("connect\n");*/
  rc = connect(data.sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  assert(rc != -1);
  /*printf("connected\n");*/

  pthread_create(&recv_tid, NULL, doRecv, &data);

  while(1)
  {
    achar = getchar();

    if (achar != 0xA)
      send(data.sockfd, &achar, sizeof(achar),0);
  }
  close(data.sockfd);
  exit(0);
}
