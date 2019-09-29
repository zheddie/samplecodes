#include <iostream>
#include "zgclasses.hpp"
namespace ZG{ namespace sotest{
    namespace {
using namespace std;

zgRegisterOperators zgreg3({
  zgOperator("zg.func2a.zgreg3.01","hello Lamda in C++ zgreg3.01"),
  zgOperator("zg.func2a.zgreg3.02","hello Lamda in C++ zgreg3.02"),
});

zgOperatorSet zgos03 = {
  "zg.func2a.zgos03.01",
  "zg.func2a.zgos03.02",
};

}
}
}