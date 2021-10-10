all: ct ctp8
ct:
	g++ -mcpu=power10 -maix64 -o ct cblastest.cpp -I ~/openblasProjects/OpenBLAS -L ~/openblasProjects/OpenBLAS -lopenblas -pthread -lgomp
ctp8:
	g++ -mcpu=power8 -maix64 -o ctp8 cblastest.cpp -I ~/openblasProjects/OpenBLAS.p8.works -L ~/openblasProjects/OpenBLAS.p8.works -lopenblas -pthread -lgomp
clean:
	rm ct ctp8
