clean:
	rm liblb2.so lb2
all: liblb2.so lb2
liblb2.so:lb2funcs.cpp
	g++ -g -fPIC -shared -o liblb2.so lb2funcs.cpp
lb2: lambda2.cpp
	export LD_LIBRARY_PATH=. && g++ -g -o lb2 lambda2.cpp -L. -llb2
run:
	export LD_LIBRARY_PATH=. && ./lb2
debug:
	export LD_LIBRARY_PATH=. && gdb ./lb2

