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
  zgOperator("zg.schema.zgreg3.01","hello Lamda in C++ zgreg3.01"),
  zgOperator("zg.schema.zgreg3.02","hello Lamda in C++ zgreg3.02"),
});
zgRegisterOperators zgreg4({1,2,3,4,5});
zgRegisterOperators zgreg5(zgOperator("zg.schema.zgreg5.02","hello Lamda in C++ zgreg5.02"));
// T1 zgt11(22);
// T1 zgt12();
// T2 zgt21("Jesse",28);
// string zgstr="hello static in namespace";
extern "C" int func2(){
    cout<<"in fuc2,t2b.age="<<t2b.getage()<<",.name:"<<t2b.getname()<<endl;
    return(0);
}
// extern "C" int getstaticint2(){
//     static int sint = 0;
//     sint ++;
//     sint2++;
//     cout<<"sint:"<<sint<<",sint2:"<<sint2<<endl;
//     return(sint2);
// }
}
}
}