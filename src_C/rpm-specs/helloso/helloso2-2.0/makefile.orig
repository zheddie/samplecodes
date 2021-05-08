#PREFIX=.
libworld.so: world.c
	gcc -fPIC -c world.c
	gcc -shared -o libworld.so world.o
libhello.so: libworld.so hello.c
	gcc -fPIC -c hello.c
	gcc -shared -Wl,-rpath ${PREFIX}/lib -o libhello.so hello.o -L. -lworld
mytest: libhello.so test.c
	gcc -fPIC -c test.c
	LD_LIBRARY_PATH=. gcc -Wl,-rpath ${PREFIX}/lib -o mytest test.o -L. -lhello
all: libhello.so libworld.so mytest
install: libhello.so libworld.so mytest
	cp libhello.so ${PREFIX}/lib/libhello.so.1.0
	ln -s libhello.so.1.0 ${PREFIX}/lib/libhello.so
	cp libworld.so ${PREFIX}/lib/libworld.so.1.0
	ln -s libworld.so.1.0 ${PREFIX}/lib/libworld.so
	cp mytest ${PREFIX}/bin
	cp world.h ${PREFIX}/include/
uninstall:
	rm  ${PREFIX}/lib/libhello.so ${PREFIX}/lib/libworld.so ${PREFIX}/bin/mytest ${PREFIX}/lib/libhello.so.1.0 ${PREFIX}/lib/libworld.so.1.0 ${PREFIX}/include/world.h
clean:
	rm mytest libhello.so libworld.so *.o
