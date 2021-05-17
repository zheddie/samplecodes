#include <pybind11/pybind11.h>
struct Widget {
    int foo( int x , int y);
    int foo( int x , int y) const;  
};

using overload_cast_  = pybind11::detail::overload_cast_impl<Args...>;



namespace py=pybind11;
PYBIND11_MODULE(zgwidget,m){
    py::class_<Widget > wdg = (m, "Widget");
    wdg.def("set", overload_cast_<int,float>(&Widget::foo))
        .def("set", overload_cast_<int,float>(&Widget::foo,py::const_))
}