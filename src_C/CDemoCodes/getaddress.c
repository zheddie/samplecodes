#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int
main()
{
char *hostname = "rchas9ed.rchland.ibm.com";
struct addrinfo hints, *res;
struct in_addr addr;
int err;

memset(&hints, 0, sizeof(hints));
hints.ai_socktype = SOCK_STREAM;
hints.ai_family = AF_INET;

if ((err = getaddrinfo(hostname, NULL, &hints, &res)) != 0) {
printf("error %d\n", err);
return 1;
}

addr.s_addr = ((struct sockaddr_in *)(res->ai_addr))->sin_addr.s_addr;

printf("ip address : %s\n", inet_ntoa(addr));

freeaddrinfo(res);

return 0;
}
