#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <poll.h>
#include <errno.h>
#include <netinet/in.h>      
#include <arpa/inet.h>       

void* tprocess1(int fdv){
        int fd=fdv;
        struct pollfd pfd;
        int rv;
        char buff[35];
        pfd.fd = fd;                                                        
        pfd.events = POLLIN | POLLERR;  //POLLIN,POLLERR are two events J9 code uses.
        printf("Poll Thread:start to poll\n");
        rv = poll(&pfd, 1, 20000);
        printf("Poll Thread:poll wake up\n");
        printf("Poll Thread:Return value is:%d\n", rv);
        printf("Poll Thread:errno is:%d\n",errno);
        printf("Poll Thread:Return event is:%d\n", pfd.revents);      
        pthread_exit(NULL);
}

void* tprocess2(int fdv){
        int fd=fdv;
        printf("Close Thread: start close fd\n");
        close(fd);
        printf("Close Thread:closed fd\n");
        pthread_exit(NULL);
}

int main(){
         pthread_t t1;
         pthread_t t2;
         int sockfd; 
         struct sockaddr_in my_addr, their_addr;   
         int sin_size; 
         sockfd = socket(AF_INET, SOCK_STREAM, 0);  
         if( sockfd<0){
           printf("Failed to create socket!\n");
           return -1;
         }
         sin_size=sizeof(struct sockaddr_in);
         memset(&my_addr,0, sizeof(struct sockaddr_in));

         my_addr.sin_family = AF_INET; 
         my_addr.sin_port = htonl(6789);  //6789 is a port I chose randomly 
         my_addr.sin_addr.s_addr = htonl(INADDR_ANY);  
         if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in))>=0)
          {
           if(listen(sockfd, 10)>=0){
             pthread_create(&t1,NULL,tprocess1,sockfd);
             pthread_create(&t2,NULL,tprocess2,sockfd);
             pthread_join(t2,NULL);
             pthread_join(t1,NULL);
             return 0;
           }
           printf("Failed to listen\n");
           return -1;
         }
         printf("Failed to bind\n");
         printf("errno is:%d\n", errno);
         return -1;        
}
