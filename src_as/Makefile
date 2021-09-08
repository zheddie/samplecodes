hello: hello.s
	gcc -o hello.o hello.s
#ld -o hello hello.o
hola: hola.s
	gcc -o hola -fPIE hola.s
tt: main.c func1.c
	/qopensys/pkgs/bin/gcc -S -maix64 main.c
	/qopensys/pkgs/bin/gcc -S -maix64 func1.c
	/qopensys/pkgs/bin/gcc -maix64 -o tt -g main.s func1.s
ttx: main.c func1.c
	xlc -S -q64 func1.c
	xlc -S -q64 main.c
	xlc -o ttx -q64 -g main.s func1.s
clean:
	rm *.o hello tt ttx func1.s main.s
