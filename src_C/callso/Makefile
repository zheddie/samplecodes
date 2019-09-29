
all:func main
func:func1.cpp
	g++ -fPIC -std=c++11 ${ZOPTS} -o func1.o -c func1.cpp
	g++  -fPIC -Wl,-brtl,-blibpath:/QOpenSys/pkgs/lib/ -std=c++11 -shared -o libfunc.so  func1.o 
main:main.c
	gcc -g -o callso main.c -L. -lfunc
clean:
	rm -rf callso *.o libfunc.so