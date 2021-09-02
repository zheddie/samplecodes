hello: hello.s
	gcc -o hello.o hello.s
#ld -o hello hello.o
hola: hola.s
	gcc -o hola -fPIE hola.s
tt: main.c func1.c
	gcc -o tt -g main.c func1.c
clean:
	rm *.o hello
