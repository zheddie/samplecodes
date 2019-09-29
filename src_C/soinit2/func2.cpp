#include <iostream>
#include "zgclasses.hpp"
namespace ZG{ namespace sotest{
    namespace {
using namespace std;
zgOp zgzgops1(40);
zgOp zgzgops2();
zgOp zgzgops3({1,2,3,4,5});
zgRegisterOperators zgreg2();
zgRegisterOperators zgreg1(123);

zgRegisterOperators zgreg3({
  zgOperator("zg.func2.zgreg3.01","hello Lamda in C++ zgreg3.01"),
  zgOperator("zg.func2.zgreg3.02","hello Lamda in C++ zgreg3.02"),
});
zgRegisterOperators zgreg4({1,2,3,4,5});
zgRegisterOperators zgreg5(zgOperator("zg.func2.zgreg5.02","hello Lamda in C++ zgreg5.02"));
// T1 zgt11(22);
// T1 zgt12();
// T2 zgt21("Jesse",28);
// string zgstr="hello static in namespace";
extern "C" int getopsidx(){
    int idx=zgzgops3.getidx();
    cout<<"in getopsidx,zgzgops3.idx="<<idx<<endl;
    return(idx);
}
zgOperatorSet zgos02 = {
  "zg.func2.zgos02.01",
  "zg.func2.zgos02.02",
};

}
}
}