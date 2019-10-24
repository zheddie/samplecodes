#include "inet.h"
#include <string.h>	// memset
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

int main(void)
{
  int	sockfd;
  struct sockaddr_in serv_addr;
  char * buffer = (char *)malloc(size);
  buffer[0] = 'a';
  buffer[size-1] = 'z';

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family		= AF_INET;
  serv_addr.sin_addr.s_addr 	= inet_addr(SERV_HOST_ADDR);
  serv_addr.sin_port		= htons(SERV_TCP_PORT);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  assert(sockfd != -1);
  
  int rc = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  assert(rc != -1);
  
  for(int i=0; i<number;i++)
  {
    if ((i % 10) == 0) printf("i = %d\n", i);

    int bytes = send(sockfd, buffer, size, 0);
    assert(bytes == size);

    memset(buffer, 0, size);	// clear buffer
    
    bytes = recv(sockfd, buffer, size, MSG_WAITALL);
    assert(bytes == size);
    /*for (int j=0;
	 j<sizeof(buffer);
	 j += recv(sockfd, buffer+j, sizeof(buffer)-j,MSG_WAITALL))
    {
      if (j!=0) printf("looping\n");
     }*/
    assert((buffer[0] == 'a') && (buffer[size-1] == 'z'));
  }
  
  printf("worked\n");
  close(sockfd);
  return 0;
}
