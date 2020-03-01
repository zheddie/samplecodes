.SUFFIXES:
.SUFFIXES:	.H .C .h .c .o 

CC		= g++

.C.o:
	$(CC) -g -c $< 


all:	server client server_unix client_unix client_udp server_udp client_menu
server:	server.o
	$(CC) -g -o $@ server.o
client:	client.o
	$(CC) -g -o $@ client.o
server_menu:	server_menu.o
	$(CC) -g -o $@ server_menu.o
client_menu:	client_menu.o 
	$(CC) -g -o $@ client_menu.o
server_unix:	server_unix.o
	$(CC) -g -o $@ server_unix.o
client_unix:	client_unix.o
	$(CC) -g -o $@ client_unix.o
server_udp:	server_udp.o
	$(CC) -g -o $@ server_udp.o
client_udp:	client_udp.o
	$(CC) -g -o $@ client_udp.o

client_menu.o: Xj9SharedHeader.H
client.o: inet.h
server.o: inet.h

clean:
	rm -f *.o
	rm -f core
	rm -rf CC\.*

	
# DO NOT DELETE THIS LINE -- make depend depends on it.
