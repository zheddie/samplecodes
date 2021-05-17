SHRFLAGS=-O3 -Wall -Werror -shared -std=c++11 -fPIC
LRPATH=-Wl,-rpath,.
PYBIND11INC=`python3 -m pybind11 --includes`
PYSOEXT=`python3-config --extension-suffix`
COMMON=${SHRFLAGS} ${PYBIND11INC} ${PYINC} 
PYINC=-I /usr/include/python3.6m -I .
CXX=g++
cppadd: cppadd.cpp cppadd.hpp
	@${CXX} ${COMMON} $^ -o lib$@.so 
zgcppadd: cppadd pb_cppadd.cpp
	g++ ${COMMON}  $^ -o $@${PYSOEXT} -L. -lcppadd 
zgpet: pb_pet.cpp
	g++ ${COMMON}  $^ -o $@${PYSOEXT} ${LRPATH} 
zgpet2: pb_pet2.cpp
	@echo "compiling $@${PYSOEXT}"
	@${CXX} ${COMMON}  $^ -o $@${PYSOEXT} ${LRPATH} 
zgvpet: pb_vpet.cpp
	@echo "compiling $@${PYSOEXT}"
	@${CXX} ${COMMON}  $^ -o $@${PYSOEXT} ${LRPATH} 

clean:
	rm *.so
zgcppadd_test:
	python3 zgcppadd_test.py