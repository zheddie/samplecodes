ifeq ($(OS),AIX)
CC=xlc
COPTS=-DAIX
else
CC=gcc
endif

all:
	${CC} -c -o zg1.o zg1.c
	${CC} -c -o zg2.o zg2.c
	
	ar -r libzg.a zg1.o zg2.o
	${CC} -c -o main.o main.c
	${CC} -o main main.o -lzg -L. 
#   ${CC} -lzg -L. -o main main.o   ===> This command would failed. Do NOT know why? A: -lzg would link the .o files in it together with main.o. So, we need to put them otgether .  
clean:
	rm zg1.o zg2.o libzg.a main main.o
