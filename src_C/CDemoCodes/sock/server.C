#include "inet.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

int main(void)
{
  int sockfd, newsockfd, childpid,num_bytes;
  socklen_t clilen;
  struct sockaddr_in cli_addr, serv_addr;
  char * buffer = (char *)malloc(size);
  
  memset(&serv_addr,0,sizeof(serv_addr));
  serv_addr.sin_family		= AF_INET;
  serv_addr.sin_port		= htons(SERV_TCP_PORT);
  serv_addr.sin_addr.s_addr	= htonl(INADDR_ANY);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  assert(sockfd != -1);
  
  int rc = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  assert(rc != -1);
  
  printf("Listening...\n");
  listen(sockfd, 5);

  for(;;)
  {
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    assert(newsockfd != -1);
    printf("accepted\n");
    
    for(int i=0;i<number;i++)
    {
      if ((i % 10) == 0) printf("i = %d\n", i);
      /*for (int j=0;
	   j<sizeof(buffer);
	   j += recv(newsockfd, buffer+j, sizeof(buffer)-j,MSG_WAITALL))
      {
	if (j!=0) printf("looping\n");
       }*/
      int bytes = recv(newsockfd, buffer, size, MSG_WAITALL);
      assert(bytes == size);

      bytes = send(newsockfd, buffer, size, 0);
      assert(bytes == size);
    }
    close(newsockfd);
  }
}

