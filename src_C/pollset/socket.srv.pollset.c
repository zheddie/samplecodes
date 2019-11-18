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
#include <sys/pollset.h>

#define TRUE             1
#define FALSE            0
#define MAXCONN          1000



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
	int fds_len = 0;
	int port = 5000;
	if(argc >1){
		port = atoi(argv[1]);
		if (port <=0 ){
			port = 5000;
		}
	}
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));
//    memset(sendBuff, '0', sizeof(sendBuff));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

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

	pollset_t psid2 = pollset_create(MAXCONN);
	pollset_t psid = pollset_create(MAXCONN);
	printf("zg.info.pollset_create,psid=%d,pid=%d\n",psid,getpid());
	if(psid <0){
		printf("ERROR: pollset create failure.psid=%d,errno=%d\n",psid,errno);
	}
	memset(pcfds,0,sizeof(pcfds));
	pcfds[0].cmd = PS_ADD;
	pcfds[0].fd = listenfd;
	pcfds[0].events = POLLIN;
	/*Add the first fd into the pollset*/
	pollset_ctl(psid,pcfds,1);
	fds_len++;
	printf("zg.info.pollset_ctl,listenfd=%d was added.\n",listenfd);
	/*************************************************************/
	/* Initialize the timeout to 3 minutes. If no               */
	/* activity after 3 minutes this program will end.           */
	/* timeout value is based on milliseconds.                   */
	/*************************************************************/
	timeout = (1 * 60 * 1000);

	/*************************************************************/
	/* Loop waiting for incoming connects or for incoming data   */
	/* on any of the connected sockets.                          */
	/*************************************************************/
	memset(fds,'9',MAXCONN*sizeof(struct pollfd));
	do {
		/***********************************************************/
		/* Call poll() and wait 3 minutes for it to complete.      */
		/***********************************************************/
		printf("Waiting on poll,fds=%p,(pollset_len=%d)...\n",fds, fds_len);
		nfds = pollset_poll(psid,fds,fds_len,timeout);
		/*rc = poll(fds, nfds, timeout);*/
		printf("zg.info.pollset_poll,nfds=%d\n",nfds);
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
			printf("INFO:fds[%d].revents=%d,events=%d,fd=%d\n", i, fds[i].revents,fds[i].events,
					fds[i].fd);
			if (fds[i].revents == 0)
				continue;

			/*check the POLLHUP, if the client closed the connection*/
			if (fds[i].revents & POLLHUP) {
				/*connection closed*/
				printf(
						"  Connection closed with POLLHUP,fds[%d].revents=%d,fd=%d\n",
						i,fds[i].revents, fds[i].fd);
				close(fds[i].fd);
				memset(pcfds,0,sizeof(pcfds));
				pcfds[0].cmd = PS_DELETE;
				pcfds[0].fd = fds[i].fd;
				pcfds[0].events = 0;
				/*Delete this fd into the pollset*/
				pollset_ctl(psid,pcfds,1);
				/*fds[i].fd = -1;*/
				fds_len--;
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
					pollset_ctl(psid,pcfds,1);
					fds_len++;

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
					fds_len--;
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
