hello: hello.s
	gcc -o hello.o -c hello.s
	ld -o hello hello.o
hola: hola.s
	gcc -o hola -fPIE hola.s
clean:
	rm *.o hello
