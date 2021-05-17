#include <pybind11/pybind11.h>
struct PolymorphicPet {
    virtual ~PolymorphicPet() = default;
};

struct PolymorphicDog : PolymorphicPet {
    std::string bark() const { return "woof!"; }
};

namespace py=pybind11;
PYBIND11_MODULE(zgvpet,m){
    py::class_<PolymorphicPet>(m, "PolymorphicPet");
    py::class_<PolymorphicDog, PolymorphicPet >(m, "PolymorphicDog")
        .def(py::init<>())
        .def("bark", &PolymorphicDog::bark);
    m.def("pet_store2",[](){ return std::unique_ptr<PolymorphicPet>(new PolymorphicDog()); });
}