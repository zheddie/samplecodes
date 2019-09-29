#include <iostream>
#include "zgclasses.hpp"
namespace torch { namespace jit{
    namespace {
using namespace std;

zgRegisterOperators reg({
  zgOperator("zg.func3.reg.01","hello Lamda in C++ reg.01"),
  zgOperator("zg.func3.reg.02","hello Lamda in C++ reg.02"),
});


}
}
}