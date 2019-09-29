
all:func2 func1 main
func1:func2
	g++ -fPIC -std=c++11 ${ZOPTS} -o func1.o -c func1.cpp
	g++  -fPIC -Wl,-brtl,-blibpath:/QOpenSys/pkgs/lib/ -std=c++11 -shared -o libfunc1.so  func1.o -L. -lfunc2 
	# -ltorch
func2:
	g++ -fPIC -std=c++11 ${ZOPTS} -o func2.o -c func2.cpp
	g++ -fPIC -std=c++11 ${ZOPTS} -o func2a.o -c func2a.cpp
	g++ -fPIC -std=c++11 -shared -o libfunc2.so func2a.o func2.o
main:
	g++ -g -o soinit main.c -ldl
clean:
	rm -rf soinit *.o libfunc1.so libfunc2.so