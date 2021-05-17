#include <pybind11/pybind11.h>
#include "cppadd.hpp"

PYBIND11_MODULE(zgcppadd,m){
	m.doc() = "zg cppadd module";
	m.def("cpp_add",&cppadd,"Try to add two int into one");
}
