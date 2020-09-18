#include <time.h>
#include <string.h>
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
  int	sockfd, newsockfd, childpid,num_bytes;
  socklen_t clilen;
  struct sockaddr_un cli_addr, serv_addr;
  char	buffer[41];
  
  assert((sockfd = socket(AF_UNIX, SOCK_STREAM, 0))!=-1);
  
  memset(&serv_addr,0,sizeof(serv_addr));
  serv_addr.sun_family	= AF_UNIX;
  strcpy(serv_addr.sun_path,"/tmp/johnsocket");
    
  assert(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))!=-1);
  printf("Listening...\n");
  listen(sockfd, 5);

  for(;;)
  {
    clilen = sizeof(cli_addr);
    assert((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen))!=-1);
    time_t s_time = time(NULL);
    for(int i=0;i<number;i++)
    {
      for (int j=0;
	   j<sizeof(buffer);
	   j += recv(newsockfd, buffer+j, sizeof(buffer)-j,0))
      {}
      send(newsockfd, buffer, sizeof(buffer), 0);
    }
    time_t e_time = time(NULL);
    double diff = difftime(e_time,s_time);
    printf("time = %f\n",diff);
    printf("read bps = %f\n",(40*number)/diff);
    close(newsockfd);
  }
}

