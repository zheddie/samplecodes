ifeq ($(OS),AIX)
CC = xlC
BIT32 = -q32
VERBOSE=
else
CC = g++
BIT32 = -m32
VERBOSE=-v
endif

PGM_NEW=new
CPPFLAGS += ${BIT32} -g
SRC_NEW = People.cpp zgHeap.cpp main.cpp
OBJS_NEW = $(SRC_NEW:.cpp=.o) 
SRCHEADER_NEW = People.h zgHeap.h 
all:${PGM_NEW}
clean:clean${PGM_NEW}
${PGM_NEW}: ${OBJS_NEW}
	${CC} ${BIT32} ${VERBOSE} -o ${PGM_NEW} ${OBJS_NEW}
clean${PGM_NEW}:
	rm ${PGM_NEW} ${OBJS_NEW}
zip${PGM_NEW}:
	tar cvf new.tar makefile ${SRC_NEW} ${SRCHEADER_NEW}
%.o:%.cpp
	${CC} ${CPPFLAGS} -c $< -o $@
OMRGLUE=$(filter /%,)
#OMRGLUE=$(filter %src,../zg ../src)
test:
	echo ==${OMRGLUE}==

