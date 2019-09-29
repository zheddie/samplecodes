
all:func main
func:func1.cpp func2.cpp func3.cpp func4.cpp
	g++ -fPIC -std=c++11 ${ZOPTS} -o func1.o -c func1.cpp
	g++ -fPIC -std=c++11 ${ZOPTS} -o func2.o -c func2.cpp
	g++ -fPIC -std=c++11 ${ZOPTS} -o func3.o -c func3.cpp
	g++ -fPIC -std=c++11 ${ZOPTS} -o func4.o -c func4.cpp
	g++  -fPIC -Wl,-brtl,-blibpath:/QOpenSys/pkgs/lib/ -std=c++11 -shared -o libfunc.so  func4.o func1.o func2.o func3.o 
	#g++	-fPIC -std=c++11 -shared -o libfunc.so  func4.o func1.o func2.o func3.o 
main:main.c
	g++ -g -o soinit main.c -L. -lfunc -L/qopensys/home/zhanggan/pytorch/build/caffe2/torch/CMakeFiles/CMakeRelink.dir/ -ltorch
clean:
	rm -rf soinit *.o libfunc.so