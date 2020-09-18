#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_UDP_PORT	6543
#define SERV_TCP_PORT   6543
/*#define SERV_HOST_ADDR 	"127.0.0.1"*/
#define SERV_HOST_ADDR 	"9.10.72.199"
int	number = 20;
/*int size = 4096 * 100;*/
int size = 4325 * 100;


