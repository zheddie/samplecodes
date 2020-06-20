#include <string.h>	// memset
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <assert.h>

int	number = 100000;


int main(void)
{
  int	sockfd;
  struct sockaddr_un serv_addr;
  char buffer[41] = "1234567890123456789012345678901234567890";

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sun_family		= AF_UNIX;
  strcpy(serv_addr.sun_path,"/tmp/johnsocket");

  assert((sockfd = socket(AF_UNIX, SOCK_STREAM, 0))!=-1);
  assert(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))!=-1);
  for(int i=0; i<number;i++)
  {
    send(sockfd, buffer, sizeof(buffer),0);
    for (int j=0;
	 j<sizeof(buffer);
	 j += recv(sockfd, buffer+j, sizeof(buffer)-j,0))
    {}
  }
  close(sockfd);
  exit(0);
}
