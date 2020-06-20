TARGETLIST=sc ss ssps sspsps libpollset.so
all: ${TARGETLIST}
sc: socketClient.c
	#gcc -o sc -pthread socketClient.c
	xlc -q64 -o sc -lpthread socketClient.c
ss: socketServer.c
	xlc -q64 -o ss -lpthread socketServer.c
ssps: socket.srv.pollset.c
	xlc -q64 -o ssps -lpthread socket.srv.pollset.c
pollset: cpollset.c
	xlc -q64 -qpic -o cpollset.o -DPOLLSET_DEBUG -c cpollset.c
	xlc -q64 -qmkshrobj -o libpollset.so cpollset.o -lpthread
sspsps: pollset
	#xlc -q64 -blibsuff:so -o sspsps -lpthread -L. -lpollset socket.srv.pollset.c -lc
	xlc -q64 -o ssps -lpthread socket.srv.pollset.c
clean:
	rm -rf ${TARGETLIST} *.o
