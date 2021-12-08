OBJ= sum
linux:
	as -a64 sum.s -o sum.o
	ld -melf64lppc sum.o -o sum
clean:
	rm *.o ${OBJ}
