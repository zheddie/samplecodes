libfunc.a: func1.o func2.o
	ar -o shr.o $?
libfunc: func1.o func2.o
	@echo "@=$@,<=$<,^=$^,?=$?"
	gcc -shared -o libfunc.so $?
main:
	gcc -o main -lfunc