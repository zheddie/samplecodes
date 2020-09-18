PREFIX ?= /zgroot
hello: hello.c
	gcc -c hello.c
	gcc -o hello hello.o
all: hello
install: hello
	mkdir -p ${PREFIX}/bin/
	cp hello ${PREFIX}/bin/
uninstall: 
	rm ${PREFIX}/bin/hello
clean:
	rm hello hello.o
