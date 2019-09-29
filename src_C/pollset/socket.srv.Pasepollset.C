#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/poll.h>
//#include <sys/pollset.h>
#include "PasePollset.H"

#define TRUE             1
#define FALSE            0
#define MAXCONN          150

#define ptr64_aix int
#define AIXINT int
#define uint64 int
#define ptr32_aix int
#define AIXULONG32 unsigned int
#define AIXLONG32 int
#define NFDS(x) (x& 0x0000FFFF)

PasePollset PasePollset::pps;

static
AIXINT pollset_create_common(ptr64_aix max){
	return PasePollset::pps.createpollset(max);
}

extern "C"
AIXINT pollset_create(uint64 max)
{

	return pollset_create_common(max);
}



static
AIXINT pollset_ctl_common(pollset_t ps, struct poll_ctl *pollctl_array,
        int64 array_length){
	return PasePollset::pps.controlpollset(ps,pollctl_array,array_length);
}

extern "C"
AIXINT pollset_ctl(pollset_t ps, struct poll_ctl *pollctl_array,
        int64 array_length)
{

	return pollset_ctl_common(ps,pollctl_array,array_length);
}


static
AIXINT pollset_poll_common(pollset_t ps, struct pollfd *polldata_array,
        int64 array_length,int64 timeout){
	struct pollfd * pf = NULL;
	int pflen = 0;
	int rt= PasePollset::pps.getpollfd(ps,&pf,&pflen);
	if(0==rt){
		for(int j = 0;j<pflen;j++){
			printf("Befor poll(pflen=%d):pf->fd=%d,pf->events=%d,pf->revents=%d\n",pflen,pf[j].fd,pf[j].events,pf[j].revents);
		}
		printf("======================================================\n");
		rt = poll(pf,
				       (AIXULONG32)pflen,
				       (AIXLONG32)timeout);
		for(int j = 0;j<pflen;j++){
			printf("After poll(rt=%d):pf->fd=%d,pf->events=%d,pf->revents=%d\n",rt,pf[j].fd,pf[j].events,pf[j].revents);
		}
		if(rt > 0){
			int idx  = 0;
			int totallen = NFDS(rt)>array_length?array_length:NFDS(rt);
			for(int i =0;i< pflen;i++){
				if(pf[i].revents == 0){
					continue;
				}else{
					if(idx< totallen){
						polldata_array[idx++] = pf[i];
					}else{	//No extra space to store the polled data.
						errno = ENOSPC;
						break;
					}
				}
			}
			return(totallen);
		}
		return(rt);
	}
	return(rt);

}

extern "C"
AIXINT pollset_poll(pollset_t ps, struct pollfd *polldata_array,
        int64 array_length , int64 timeout)
{

	return pollset_poll_common(ps,polldata_array,array_length,timeout);
}

static
AIXINT pollset_destroy_common(pollset_t ps){
	return PasePollset::pps.destroypollset(ps);
}
extern "C"
AIXINT pollset_destroy(pollset_t ps)
{

	return pollset_destroy_common(ps);
}

static
AIXINT pollset_query_common(pollset_t ps,struct pollfd *querydata){
	return PasePollset::pps.querypollset(ps,querydata);
}
extern "C"
AIXINT pollset_query(pollset_t ps,struct pollfd *querydata)
{

	return pollset_query_common(ps,querydata);
}




int main(int argc, char *argv[]) {
	int listenfd = 0, connfd = 0, idx = 0, n = 0, on = 1;
	struct sockaddr_in serv_addr;
	char recvBuff[1024];
//    char sendBuff[1025];
	time_t ticks;
	int timeout, rcvCount = 0;

	struct pollfd fds[MAXCONN];
	struct poll_ctl pcfds[1];

	int nfds = 1, current_size = 0, end_server = FALSE, new_sd = -1, close_conn,
			 i, j;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));
//    memset(sendBuff, '0', sizeof(sendBuff));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5000);

	int rc = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *) &on,
			sizeof(on));
	if (rc < 0) {
		perror("ERROR: setsockopt() failed");
		close(listenfd);
		exit(-1);
	}

	rc = ioctl(listenfd, FIONBIO, (char *) &on);
	if (rc < 0) {
		perror("ERROR:ioctl() failed");
		close(listenfd);
		exit(-1);
	}

	bind(listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));

	listen(listenfd, MAXCONN);

	pollset_t psid = pollset_create(MAXCONN);
	if(psid <0){
		printf("ERROR: pollset create failure.psid=%d,errno=%d\n",psid,errno);
	}
	memset(pcfds,0,sizeof(pcfds));
	pcfds[0].cmd = PS_ADD;
	pcfds[0].fd = listenfd;
	pcfds[0].events = POLLIN;
	/*Add the first fd into the pollset*/
	pollset_ctl(psid,pcfds,1);

	/*************************************************************/
	/* Initialize the timeout to 3 minutes. If no               */
	/* activity after 3 minutes this program will end.           */
	/* timeout value is based on milliseconds.                   */
	/*************************************************************/
	timeout = (3 * 60 * 1000);

	/*************************************************************/
	/* Loop waiting for incoming connects or for incoming data   */
	/* on any of the connected sockets.                          */
	/*************************************************************/
	do {
		/***********************************************************/
		/* Call poll() and wait 3 minutes for it to complete.      */
		/***********************************************************/
		printf("Waiting on poll(pid=%d)...\n",getpid());
		nfds = pollset_poll(psid,fds,MAXCONN,timeout);
		/*rc = poll(fds, nfds, timeout);*/

		/***********************************************************/
		/* Check to see if the poll call failed.                   */
		/***********************************************************/
		if (nfds < 0) {
			perror("  pollset_poll() failed");
			break;
		}

		/***********************************************************/
		/* Check to see if the 3 minute time out expired.          */
		/***********************************************************/
		if (nfds == 0) {
			printf("  pollset_poll() timed out.  End program.\n");
			break;
		}

		/***********************************************************/
		/* One or more descriptors are readable.  Need to          */
		/* determine which ones they are.                          */
		/***********************************************************/
		current_size = nfds;
		for (i = 0; i < current_size; i++) {
			/*********************************************************/
			/* Loop through to find the descriptors that returned    */
			/* POLLIN and determine whether it's the listening       */
			/* or the active connection.                             */
			/*********************************************************/
//			printf("INFO:fds[%d].revents=%d,fd=%d\n", i, fds[i].revents,
//					fds[i].fd);
			if (fds[i].revents == 0)
				continue;

			/*check the POLLHUP, if the client closed the connection*/
			if (fds[i].revents & POLLHUP) {
				/*connection closed*/
//				printf(
//						"  Connection closed with POLLHUP,fds[i].revents=%d,fd=%d\n",
//						fds[i].revents, fds[i].fd);
				close(fds[i].fd);
				memset(pcfds,0,sizeof(pcfds));
				pcfds[0].cmd = PS_DELETE;
				pcfds[0].fd = fds[i].fd;
				pcfds[0].events = 0;
				/*Delete this fd into the pollset*/
				pollset_ctl(psid,pcfds,1);
				/*fds[i].fd = -1;*/

				continue;
			}
			/*********************************************************/
			/* If revents is not POLLIN, it's an unexpected result,  */
			/* log and end the server.                               */
			/*********************************************************/
			//if(fds[i].revents != POLLIN ) //|| fds[i].revents != POLLOUT)
			if (!(fds[i].revents & POLLIN) && !(fds[i].revents & POLLOUT)) {
				printf("  Error! revents = %d,fd= %d\n", fds[i].revents,
						fds[i].fd);
				end_server = TRUE;
				break;
			}
			if (fds[i].fd == listenfd && fds[i].revents == POLLIN) {
				/*******************************************************/
				/* Listening descriptor is readable.                   */
				/*******************************************************/
				printf("  Listening socket is readable\n");

				/*******************************************************/
				/* Accept all incoming connections that are            */
				/* queued up on the listening socket before we         */
				/* loop back and call poll again.                      */
				/*******************************************************/
				do {
					/*****************************************************/
					/* Accept each incoming connection. If              */
					/* accept fails with EWOULDBLOCK, then we            */
					/* have accepted all of them. Any other             */
					/* failure on accept will cause us to end the        */
					/* server.                                           */
					/*****************************************************/
					new_sd = accept(listenfd, NULL, NULL);
					if (new_sd < 0) {
						if (errno != EWOULDBLOCK) {
							perror("  accept() failed");
							end_server = TRUE;
						}
						break;
					}

					/*****************************************************/
					/* Add the new incoming connection to the            */
					/* pollfd structure                                  */
					/*****************************************************/
					printf("  New incoming connection - %d\n", new_sd);
					memset(pcfds,0,sizeof(pcfds));
					pcfds[0].cmd = PS_ADD;
					pcfds[0].fd = new_sd;
					pcfds[0].events = POLLIN;
					/*Add one new fd into the pollset*/
					int rt = pollset_ctl(psid,pcfds,1);
					if(rt != 0){
						printf("ERROR:pollset_ctl failed. rt=%d,errno=%d\n",rt,errno);
						end_server=TRUE;
						break;
					}

					/*****************************************************/
					/* Loop back up and accept another incoming          */
					/* connection                                        */
					/*****************************************************/
				} while (new_sd != -1);
			} else if (fds[i].fd == listenfd && fds[i].revents != POLLIN) {
				printf("ERROR: listenfd(%d) is ready for POLLOUT(%d)?\n",
						listenfd, fds[i].revents);
			}

			/*********************************************************/
			/* This is not the listening socket, therefore an        */
			/* existing connection must be readable                  */
			/*********************************************************/

			else {
				printf("  Descriptor %d is readable/writable?\n", fds[i].fd);
				close_conn = FALSE;
				/*******************************************************/
				/* Receive all incoming data on this socket            */
				/* before we loop back and call poll again.            */
				/*******************************************************/

				do {
					if (fds[i].revents & POLLIN) {
						/*****************************************************/
						/* Receive data on this connection until the         */
						/* recv fails with EWOULDBLOCK. If any other        */
						/* failure occurs, we will close the                 */
						/* connection.                                       */
						/*****************************************************/
						rc = recv(fds[i].fd, recvBuff, sizeof(recvBuff), 0);
						if (rc < 0) {
							if (errno != EWOULDBLOCK) {
								perror("  recv() failed");
								close_conn = TRUE;
							}
							break;
						}

						/*****************************************************/
						/* Check to see if the connection has been           */
						/* closed by the client                              */
						/*****************************************************/
						if (rc == 0) {
							printf("  Connection closed\n");
							close_conn = TRUE;
							break;
						}

						/*****************************************************/
						/* Data was received                                 */
						/*****************************************************/
						int len = rc;
						rcvCount++;
						printf("  fd=%d:%d bytes received:%s--rcvCount=%d\n", fds[i].fd, len,
								recvBuff,rcvCount);

						sprintf(recvBuff, "Server echo to %d", fds[i].fd);
						rc = send(fds[i].fd, recvBuff, strlen(recvBuff), 0);
						printf("%s\n", recvBuff);
						if (rc < 0) {
							printf("%d:ERROR,Send echo error!\n", fds[i].fd);
						} else if (rc == 0) {
							printf("%d:ERROR,Connection closed?\n", fds[i].fd);
						}
						/*****************************************************/
						/* Echo the data back to the client                  */
						/*****************************************************/
//            rc = send(fds[i].fd, buffer, len, 0);
//            if (rc < 0)
//            {
//              perror("  send() failed");
//              close_conn = TRUE;
//              break;
//            }
					} else {
						sprintf(recvBuff, "Server echo:%d", rcvCount);
						rc = send(fds[i].fd, recvBuff, strlen(recvBuff), 0);
						printf("tobe echo: %s\n", recvBuff);
						if (rc < 0) {
							printf("%d:ERROR,Send echo error!\n", rcvCount);
						} else if (rc == 0) {
							printf("%d:ERROR,Connection closed?\n", rcvCount);
						}
					}
				} while (TRUE);

				/*******************************************************/
				/* If the close_conn flag was turned on, we need       */
				/* to clean up this active connection. This           */
				/* clean up process includes removing the              */
				/* descriptor.                                         */
				/*******************************************************/
				if (close_conn) {
					close(fds[i].fd);
					memset(pcfds,0,sizeof(pcfds));
					pcfds[0].cmd = PS_DELETE;
					pcfds[0].fd = fds[i].fd;
					pcfds[0].events = 0;
					/*Delete this fd into the pollset*/
					pollset_ctl(psid,pcfds,1);
					/*fds[i].fd = -1;*/

				}

			} /* End of existing connection is readable             */
		} /* End of loop through pollable descriptors              */


	} while (end_server == FALSE); /* End of serving running.    */
	rc = pollset_destroy(psid);
	if(rc != 0){
		printf("ERROR: pollset_destroy failure with rc=%d,errno=%d\n",rc,errno);
	}

}
