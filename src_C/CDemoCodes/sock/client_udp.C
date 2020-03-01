#include "inet.h"
#include <string.h>	// memset
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/ioctl.h>
#ifdef __AIX__
#include <net/netopt.h>
#endif
#include <errno.h>

int main(void)
{
  int	sockfd;
  struct sockaddr_in serv_addr;
  char buffer[41] = "1234567890123456789012345678901234567890";
  int buf_size = 64*1024+1;
  //struct optreq oq;                                    /*ibm@112184*/
  int *bufsize, maxbuf;

  
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family		= AF_INET;
  serv_addr.sin_addr.s_addr 	= inet_addr(SERV_HOST_ADDR);
  serv_addr.sin_port		= htons(SERV_TCP_PORT);

  assert((sockfd = socket(AF_INET, SOCK_DGRAM, 0))!=-1);
  assert(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))!=-1);

  /*strcpy(oq.name, "sb_max");                           
  oq.getnext = 0;                                     
  if (ioctl(sockfd, SIOCGNETOPT, (caddr_t)&oq) != 0) {     
    printf("bad ioctl %d\n", errno);
    return -1;                                       
  }                                                    
  maxbuf = atoi(oq.data);                         
  printf("maxbuf = %d\n", maxbuf);
  
  bufsize = &buf_size;
  if (*bufsize > maxbuf)
  {
    *bufsize = maxbuf;
  }
*/


  
 /* int randy = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &buf_size, 4);
  printf("randy = %d errno = %d\n", randy, errno);
  assert(randy == 0);*/

  for(int i=0; i<3;i++)
  {
    //send(sockfd, buffer, sizeof(buffer),0);
    //int bytes = sendto(sockfd, buffer, sizeof(buffer),0,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
    int bytes = sendto(sockfd, buffer, sizeof(buffer),0, NULL, 0);
    if (bytes == -1)
    {
      printf("errno = %d\n", errno);
    }
    printf("bytes = %d\n", bytes);
  }
  close(sockfd);
  exit(0);
}
