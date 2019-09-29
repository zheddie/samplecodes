CPP := g++
CFLAG := -g -std=c++11 -fPIC
libfoo.so: libfoo.o
	$(CPP) $(CFLAG)  -shared  -o $@ $+ -lpthread

%.o : %.cc %.h
	$(CPP) $(CFLAG) -o $@ -c $<

tls: libfoo.so main.o
	$(CPP) $(CFLAG) -o $@ -L. -lfoo main.o
clean:
	rm -rf *.o *.so tls
prog:
	g++ a.cpp -c -o a.o -std=c++11
	g++ b.cpp -c -o b.o -std=c++11
	g++ a.o b.o -o prog -std=c++11
foo: fooa.cpp foob.cpp
	g++ fooa.cpp -c -o fooa.o -std=c++11
	g++ foob.cpp -c -o foob.o -std=c++11
	g++ fooa.o foob.o -o foo -std=c++11
