all:func1 func2 main mainthread
func1: func1.cpp
	g++ -fPIC -std=c++11 ${ZOPTS} -o func1.o -c func1.cpp
	#g++  -fPIC -Wl,-brtl,-blibpath:/QOpenSys/pkgs/lib/ -std=c++11 -shared -o libfunc1.so  func1.o
	g++  -fPIC -std=c++11 -shared -o libfunc1.so  func1.o
func2: func2.cpp
	g++ -fPIC -std=c++11 ${ZOPTS} -o func2.o -c func2.cpp
	g++  -fPIC -std=c++11 -shared -o libfunc2.so  func2.o
main: libfunc1.so main.c
	gcc -g -o soinit main.c -pthread -ldl
mainthread: libfunc1.so libfunc2.so  mainthread.c
	gcc -g -o soinitthr mainthread.c -pthread -ldl
clean:
	rm -rf soinit soinitthr *.o libfunc1.so libfunc2.so 
