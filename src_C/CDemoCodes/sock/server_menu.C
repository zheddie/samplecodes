#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_TCP_PORT   6543

int main(void)
{
  int sockfd, newsockfd, childpid,num_bytes;
  socklen_t clilen;
  struct sockaddr_in cli_addr, serv_addr;
  char buffer[4096];
  char * str;
  int rc;
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  assert(sockfd != -1);
  
  memset(&serv_addr,0,sizeof(serv_addr));
  serv_addr.sin_family		= AF_INET;
  serv_addr.sin_port		= htons(SERV_TCP_PORT);
  serv_addr.sin_addr.s_addr	= htonl(INADDR_ANY);
  
  rc = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  assert(rc == 0);
  printf("listen\n");
  rc = listen(sockfd, 5);
  assert(rc == 0);
  
  while(1)
  {
    clilen = sizeof(cli_addr);
    printf("accepting...\n");
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    assert(newsockfd != -1);
    printf("accepted\n");
    int done = 0;

    while(!done)
    {
      str = "Hello from J9\n\n";
      send(newsockfd, str, strlen(str), 0);
      str = "Menu\n";
      send(newsockfd, str, strlen(str), 0);
      str = "1. nothing\n";
      send(newsockfd, str, strlen(str), 0);
      str = "9. Disconnect\n";
      send(newsockfd, str, strlen(str), 0);

      recv(newsockfd, buffer, sizeof(buffer),0);
      printf("got %c\n",buffer[0]);
      switch(buffer[0])
      {
	  case '1':
	      printf("case 1\n");
	      break;
	  case '9':
	      done = 1;
	      break;
      }
    }
    close(newsockfd);
  }
}

