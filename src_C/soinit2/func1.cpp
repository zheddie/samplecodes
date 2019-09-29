#include <iostream>
#include "zgclasses.hpp"


namespace ZG{namespace sotest{
    namespace {
using namespace std;


zgRegisterOperators zgreg3({
  zgOperator("zg.func1.zgreg3.01","hello Lamda in C++ zgreg3.01"),
  zgOperator("zg.func1.zgreg3.02","hello Lamda in C++ zgreg3.02"),
});

extern "C" int callfunc2(){
    
    int idx = getopsidx();
    cout<<"In func1.cpp.callfunc2().cp0,idx="<<idx<<endl;
    return(0);

}
zgOperatorSet zgos01 = {
  "zg.func1.zgos01.01",
  "zg.func1.zgos01.02",
};
}
}
}