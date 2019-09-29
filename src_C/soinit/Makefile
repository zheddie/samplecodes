
all:
	g++ -fPIC -std=c++11 ${ZOPTS} -o func1.o -c func1.cpp
	g++ -fPIC -std=c++11 ${ZOPTS} -o func2.o -c func2.cpp
	g++ -fPIC -std=c++11 ${ZOPTS} -o func3.o -c func3.cpp
	g++ -fPIC -std=c++11 ${ZOPTS} -o func4.o -c func4.cpp
	g++ -fPIC -std=c++11 ${ZOPTS} -o func5.o -c func5.cpp
	g++  -fPIC --std=c++11 -shared -o libfunc1.so  func2.o func1.o func3.o func4.o func5.o
	g++ -fPIC --std=c++11 -shared -o libfunc2.so func1.o
	g++ -o soinit2 main2.cpp -ldl
func2:
	g++ -fPIC -std=c++11 ${ZOPTS} -o func2.o -c func2.cpp
	g++ -fPIC --std=c++11 -shared -o libfunc2.so func2.o
	g++ -o soinit3 main3.c -ldl
xlCall:
	xlC -G -o libfunc1.so func1.cpp
	xlC -o soinit2 main2.cpp
