CPPFLAGS=-g
people:People.cpp People.h
	g++ ${CPPFLAGS}  -o people.so People.cpp
us:
	g++ ${CPPFLAGS} -o ush.o -c IASPUserSpaceHeader.C
	g++ ${CPPFLAGS} -o use.o -c IASPUserSpaceEntry.C
	g++ ${CPPFLAGS} -o uush.o -c UserSpaceHeader.C
	g++ ${CPPFLAGS} -o mic.o -c mrdbIfsCfg.C
	g++ ${CPPFLAGS} -o libus.so -shared ush.o use.o uush.o mic.o
clean:
	rm *.o *.so tc dem
tc:
	g++ ${CPPFLAGS} TestCases.cpp  -o tc -L. -lus
dem:
	g++ -o dem DemoCases.cpp
