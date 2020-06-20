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
  char buffer[41];
  
  assert((sockfd = socket(AF_INET, SOCK_DGRAM, 0))!=-1);
  
  memset(&serv_addr,0,sizeof(serv_addr));
  serv_addr.sin_family		= AF_INET;
  serv_addr.sin_port		= htons(SERV_TCP_PORT);
  serv_addr.sin_addr.s_addr	= htonl(INADDR_ANY);
  
  assert(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))!=-1);
  printf("Listening...\n");
  listen(sockfd, 5);

  for(;;)
  {
    recvfrom(sockfd, buffer, sizeof(buffer),0, (struct sockaddr *) &cli_addr, &clilen);
    printf("data = %s\n", buffer);
  }
}

