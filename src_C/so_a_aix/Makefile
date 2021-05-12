func.a: func1.o func2.o
	ar -o shr.o $?
func: func1.o func2.o func3.o
	@echo "@=$@,<=$<,^=$^,?=$?"
	gcc -shared -fPIC -fvisibility=hidden -o libfunc.so $?
main:
	gcc -o main main.c  -L. -lfunc
clean:
	rm *.o main libfunc.so