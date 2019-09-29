#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main()
{
	int sockfd, cnt, i = 1;
	struct sockaddr_in  serv_addr;

	bzero((char *)&serv_addr, sizeof (serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_addr.sin_port = htons(10002);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		exit(1);
	if (fcntl(sockfd, F_SETFL, FNONBLOCK) < 0)
		exit(1);
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof
				(serv_addr)) < 0 && errno != EINPROGRESS)
		exit(1);
	printf("zg.sockfd=%d\n",sockfd);
	for (cnt=0; cnt<4; cnt++) {
		fd_set  readfds, writefds;

		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);
		FD_ZERO(&writefds);
		FD_SET(sockfd, &writefds);
		int rt = select(sockfd + 1, &readfds, &writefds, NULL, NULL);
		printf("zg.return from select: %d\n",rt);
//		if (select(sockfd + 1, &readfds, &writefds, NULL,
		if (rt < 0)
			exit(1);
		printf("Iteration %d ==============\n", i);
		printf("FD_ISSET(sockfd, &readfds) == %d\n",
				FD_ISSET(sockfd, &readfds));
		printf("FD_ISSET(sockfd, &writefds) == %d\n",
				FD_ISSET(sockfd, &writefds));
		i++;
	}
	return 0;
}
