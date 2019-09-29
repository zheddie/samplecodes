file1.o:file1.c
	gcc -o file1.o -c file1.c
file2.o:file2.c
	gcc -o file2.o -c file2.c
file3.o:file3.c
	gcc -o file3.o -c file3.c
main.o:main.c
	gcc -o main.o -c main.c
libfile.a:file2.o
	ar qc libfile.a file2.o
	ranlib libfile.a
libfiles.so:file1.o libfile.a file3.o
	gcc -shared -u func2 -o libfiles.so -fPIC file1.o libfile.a file3.o
main:main.o libfiles.so
	gcc -o main main.o -L. -lfiles
clean:
	rm main *.o *.so *.a
	
