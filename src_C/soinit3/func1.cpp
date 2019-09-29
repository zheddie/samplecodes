#include <iostream>
#include "zgclasses.hpp"

int getopsidx(){
    return(100);
}

namespace torch {namespace jit{
    namespace {
using namespace std;


zgRegisterOperators reg({
  zgOperator("zg.func1.reg.01","hello Lamda in C++ reg.01"),
  zgOperator("zg.func1.reg.02","hello Lamda in C++ reg.02"),
});


}
}
}