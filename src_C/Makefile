OBJ_ALL=liboncecaller.so *.o onceexe2 once _C.so onceexe S.so caller
#LDFLAGS=-maix64 -Wl,-brtl,-blibpath:/QOpenSys/pkgs/lib:/usr/lib
ifdef PASEBUILD
	LDFLAGS=-maix64 -Wl,-brtl,-blibpath:/QOpenSys/pkgs/lib:/QOpenSys/usr/lib
	PASEFLAG=-pthread 
else
	LDFLAGS=-Wl,-rpath,/usr/lib/
	LINUXFLAG=-ldl
endif


once:once.cpp
	g++ -std=c++11 -pthread -o once once.cpp
cleanonce:
	rm once
onceso:onceso.cpp
#	gcc -fPIC -pthread -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O3 -Wall -O2 -g -maix64 -O2 -g -maix64 -I/QOpenSys/home/zhanggan/pytorch -I/QOpenSys/home/zhanggan/pytorch/torch/lib/tmp_install/include -I/QOpenSys/home/zhanggan/pytorch/torch/lib/tmp_install/include/TH -I/QOpenSys/home/zhanggan/pytorch/torch/lib/tmp_install/include/THNN -I/QOpenSys/home/zhanggan/pytorch/torch/lib/tmp_install/include/ATen -I/QOpenSys/home/zhanggan/pytorch/third_party/pybind11/include -I/QOpenSys/home/zhanggan/pytorch/torch/csrc -Ibuild/third_party -I/QOpenSys/pkgs/lib/python3.6/site-packages/numpy/core/include -I/QOpenSys/pkgs/include/python3.6m -c onceso.cpp  -o onceso.o -D_THP_CORE -DONNX_NAMESPACE=onnx_torch -std=c++11 -Wall -Wextra -Wno-strict-overflow -Wno-unused-parameter -Wno-missing-field-initializers -Wno-write-strings -Wno-zero-length-array -Wno-unknown-pragmas -Wno-deprecated-declarations -fno-strict-aliasing -Wno-missing-braces -Wno-stringop-overflow -Wno-attributes -DUSE_NUMPY
#	/QOpenSys/pkgs/lib/python3.6/config-3.6m/ld_so_aix g++ -pthread -bI:/QOpenSys/pkgs/lib/python3.6/config-3.6m/python.exp onceso.o -L/QOpenSys/home/zhanggan/pytorch/torch/lib -L/QOpenSys/pkgs/lib -lshm -o _C.so -lcaffe2 -lc10 -ltorch -Wl,-brtl,-blibpath:/QOpenSys/pkgs/lib:/usr/lib/
#	/QOpenSys/pkgs/lib/python3.6/config-3.6m/ld_so_aix g++ -pthread -bI:/QOpenSys/pkgs/lib/python3.6/config-3.6m/python.exp onceso.o -L/usr/lib/ -lc  -o _C.so ${LDFLAGS}
	gcc -fPIC -std=c++11 ${FTLS} -pthread -o onceso.o -c onceso.cpp
	g++ -fPIC -pthread -shared ${LDFLAGS} -o _C.so onceso.o 
cleanonceso:
	rm onceso.o _C.so
onceexe:_C.so onceexe.c
	gcc -std=c99 -c onceexeso.c -o onceexeso.o -pthread
	gcc -shared -ftls-model=initial-exec -pthread ${LDFLAGS} -ldl -o liboncecaller.so onceexeso.o
	cp liboncecaller.so /qopensys/pkgs/lib/
	gcc -ftls-model=initial-exec -std=c99 -pthread ${LDFLAGS} -L. -loncecaller -o onceexe  onceexe.c
onceexe2:_C.so onceexe.cpp
	g++ -ftls-model=initial-exec -std=c++11 -c onceexeso.cpp -o onceexeso.o ${PASEFLAG}
	#g++ -fPIC -std=c++11 -c onceexeso.cpp -o onceexeso.o ${PASEFLAG}
#g++ -ftls-model=initial-exec -pthread -shared ${LDFLAGS} -o liboncecaller.so onceexeso.o
	g++ -ftls-model=initial-exec -pthread -shared ${LDFLAGS} -ldl -o liboncecaller.so onceexeso.o
	g++ -ftls-model=initial-exec -std=c++11 ${LINUXFLAG} ${LDFLAGS} -L. -loncecaller -o onceexe2 onceexe.cpp 
cleanonceexe:
	rm onceexe
cppso: Makefile cppso.cpp
	g++ -std=c++11 -fPIC -o cppso.o -c cppso.cpp
	g++ -shared ${LDFLAGS} -o S.so cppso.o
caller: Makefile callcppso.c
	gcc -c -std=c99 -o caller.o callcppso.c
	gcc -o caller caller.o ${LINUXLIB}
	#g++ -c -o caller.o callcppso.c
	#g++ -o caller caller.o ${LINUXLIB}
cleanall:
	rm ${OBJ_ALL}
