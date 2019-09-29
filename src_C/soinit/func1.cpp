#include <iostream>
#include "zgclasses.hpp"
namespace ZG{namespace sotest{
    namespace {
using namespace std;


zgRegisterOperators zgreg3({
  zgOperator("zg.schema.func1.zgreg3.01","hello Lamda in C++ zgreg3.01"),
  zgOperator("zg.schema.func1.zgreg3.02","hello Lamda in C++ zgreg3.02"),
});
// T2 zgt2b("Tim",58) ;
// int zgsint2 = 8;
// extern "C" int func1(){
// 	cout<<"in fuc1,t2b.age="<<t2b.getage()<<",.name:"<<t2b.getname()<<endl;
// 	return(0);
// }
// extern "C" int getstaticint(){
// 	static int sint = 0;
// 	sint ++;
// 	sint2++;
// 	cout<<"sint:"<<sint<<",sint2:"<<sint2<<endl;
// 	return(sint2);
// }
}
}
}