
libworld.so: world.c
	gcc -fPIC -c world.c
	gcc -shared -o libworld.so.${VERSION} world.o
	ln -s libworld.so.${VERSION} libworld.so
libhello.so: libworld.so hello.c
	gcc -fPIC -c hello.c
	gcc -shared -Wl,-rpath,${PREFIX}/lib64 -o libhello.so.${VERSION} hello.o -L. -lworld
	ln -s libhello.so.${VERSION} libhello.so
mytest: libhello.so test.c
	gcc -fPIC -c test.c
	LD_LIBRARY_PATH=. gcc -Wl,-rpath,${PREFIX}/lib64 -o mytest test.o -L. -lhello
all: libhello.so libworld.so mytest
install: libhello.so libworld.so mytest
	cp libhello.so.${VERSION} ${PREFIX}/lib64/
	cp -d libhello.so ${PREFIX}/lib64/
	cp libworld.so.${VERSION} ${PREFIX}/lib64/
	cp -d libworld.so ${PREFIX}/lib64/
	cp mytest ${PREFIX}/bin
	cp world.h ${PREFIX}/include/
uninstall:
	rm  ${PREFIX}/lib64/libhello.so ${PREFIX}/lib64/libworld.so ${PREFIX}/bin/mytest ${PREFIX}/lib64/libhello.so.${VERSION} ${PREFIX}/lib64/libworld.so.${VERSION} ${PREFIX}/include/world.h
clean:
	rm mytest libhello.so* libworld.so* *.o
