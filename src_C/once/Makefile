OBJ_ALL=once onceso.o _C.so onceexe
LDFLAGS=-Wl,-brtl,-blibpath:/QOpenSys/pkgs/lib:/usr/lib
once:once.cpp
	g++ -std=c++11 -pthread -o once once.cpp
cleanonce:
	rm once
onceso:onceso.cpp
	gcc -pthread -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O3 -Wall -O2 -g -maix64 -O2 -g -maix64 -I/QOpenSys/home/zhanggan/pytorch -I/QOpenSys/home/zhanggan/pytorch/torch/lib/tmp_install/include -I/QOpenSys/home/zhanggan/pytorch/torch/lib/tmp_install/include/TH -I/QOpenSys/home/zhanggan/pytorch/torch/lib/tmp_install/include/THNN -I/QOpenSys/home/zhanggan/pytorch/torch/lib/tmp_install/include/ATen -I/QOpenSys/home/zhanggan/pytorch/third_party/pybind11/include -I/QOpenSys/home/zhanggan/pytorch/torch/csrc -Ibuild/third_party -I/QOpenSys/pkgs/lib/python3.6/site-packages/numpy/core/include -I/QOpenSys/pkgs/include/python3.6m -c onceso.cpp  -o onceso.o -D_THP_CORE -DONNX_NAMESPACE=onnx_torch -std=c++11 -Wall -Wextra -Wno-strict-overflow -Wno-unused-parameter -Wno-missing-field-initializers -Wno-write-strings -Wno-zero-length-array -Wno-unknown-pragmas -Wno-deprecated-declarations -fno-strict-aliasing -Wno-missing-braces -Wno-stringop-overflow -Wno-attributes -DUSE_NUMPY
#	/QOpenSys/pkgs/lib/python3.6/config-3.6m/ld_so_aix g++ -fPIC -pthread -bI:/QOpenSys/pkgs/lib/python3.6/config-3.6m/python.exp onceso.o -L/QOpenSys/home/zhanggan/pytorch/torch/lib -L/QOpenSys/pkgs/lib -lshm -o _C.so -lcaffe2 -lc10 -ltorch -Wl,-brtl,-blibpath:/QOpenSys/pkgs/lib:/usr/lib/
	/QOpenSys/pkgs/lib/python3.6/config-3.6m/ld_so_aix g++ -fPIC -pthread -bI:/QOpenSys/pkgs/lib/python3.6/config-3.6m/python.exp onceso.o -L/QOpenSys/home/zhanggan/pytorch/torch/lib -L/QOpenSys/pkgs/lib -lshm -o _C.so -lcaffe2 -lc10 -ltorch -Wl,-brtl,-blibpath:/QOpenSys/pkgs/lib:/usr/lib/
	#gcc -std=c++11 ${FTLS} -pthread -o onceso.o -c onceso.cpp
	#g++ -pthread -shared ${FTLS} ${LDFLAGS} -o _C.so onceso.o 
cleanonceso:
	rm onceso.o _C.so
onceexe:_C.so onceexe.cpp
	g++ -std=c++11 -pthread ${LDFLAGS} -o onceexe  onceexe.cpp
cleanonceexe:
	rm onceexe
cleanall:
	rm ${OBJ_ALL}
