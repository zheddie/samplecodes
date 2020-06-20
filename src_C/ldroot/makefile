libworld.so: world.c
	gcc -fPIC -c world.c
	gcc -shared -o libworld.so.2 world.o
	ln -s libworld.so.2 libworld.so
libhello.so: libworld.so hello.c
	gcc -fPIC -c hello.c
	gcc -shared -Wl,-rpath ${PREFIX}/lib64 -o libhello.so.2 hello.o -L. -lworld
	ln -s libhello.so.2 libhello.so
mytest: libhello.so test.c
	gcc -fPIC -c test.c
	LD_LIBRARY_PATH=. gcc -Wl,-rpath ${PREFIX}/lib64 -o mytest test.o -L. -lhello
all: libhello.so libworld.so mytest
install: libhello.so libworld.so mytest
	cp libhello.so.2 ${PREFIX}/lib64/
	cp -d libhello.so ${PREFIX}/lib64/
	cp libworld.so.2 ${PREFIX}/lib64/
	cp -d libworld.so ${PREFIX}/lib64/
	cp mytest ${PREFIX}/bin
	cp world.h ${PREFIX}/include/
uninstall:
	rm  ${PREFIX}/lib64/libhello.so ${PREFIX}/lib64/libworld.so ${PREFIX}/bin/mytest ${PREFIX}/lib64/libhello.so.1 ${PREFIX}/lib64/libworld.so.1 ${PREFIX}/include/world.h
clean:
	rm mytest libhello.so libworld.so libhello.so.1 libworld.so.1 *.o
