#include <iostream>
#include <vector>
#include <string>

namespace ZG{namespace sotest{
    namespace {
using namespace std;
// struct T1{
// public:
// 	T1(){
// 		age = -1;
// 		cout<<"in T1::T1(),age="<<age<<",this="<<this <<endl;	
// 	}
// 	T1(int ag){
// 		age = ag;
// 		cout<<"in T1::T1(),age="<<age<<",this="<<this <<endl;	
// 	}
// 	int getage(){return age;}
// private:
// 	int age;
// };
// struct T2:public T1{
// public:
// 	T2(){
// 		strcpy(name,"zg");
// 		cout<<"in T2::T2(),name="<<name<<",age="<<getage()<<",this="<<this<<endl;
// 	}
// 	T2(char * nm,int ag):T1(ag){
// 		strcpy(name,nm);
// 		cout<<"in T2::T2(),name="<<name<<",age="<<getage()<<",this="<<this<<endl;
// 	}
// 	char * getname(){return name;}
// private:
// 	char name[10];
// };

struct zgOp{
public:
  zgOp()=default;
  // zgOp() {
  // 	idx3 = -1;
  //   cout<<"zg.zgclasses.hpp.zgOp.cp2,idx3="<<idx3<<"\n";
    
  // };
  zgOp(int i) {
  	idx3 = i;
    cout<<"zg.zgclasses.hpp.zgOp.cp1,idx3="<<idx3<<"\n";
    
  };
  zgOp(std::vector<int> zlist){
  	idx3=0;
  	for(int & each: zlist){
  		idx3+= each;
  	}
  	cout<<"zg.zgclasses.hpp.zgOp.cp3,idx3="<<idx3<<"\n";
  }
private:
    int idx3;
};
struct  zgOperator {
public:
  
  zgOperator(const std::string& schema, const std::string& lambda) {
    std::cout<<"zg.zgdefs.hpp.zgOperator_struct.zgOperator.cp0,schema="<<schema<<",lambda="<<lambda<<"\n";
  };
private:
    int idx1;
};

struct  zgRegisterOperators {
public:
  zgRegisterOperators() = default;

  /// Registers a vector of already created `Operator`s.
  zgRegisterOperators(std::vector<zgOperator> operators) {
    int idx2 = 0;
    for (zgOperator& o : operators) {
      std::cout<<"zg.zgdefs.hpp.zgRegisterOperators_struct.zgRegisterOperators.cp3,idx2="<<idx2++<<"\n";
    }
  }
  zgRegisterOperators(zgOperator x ) {
  	idx2=sizeof(x);
  	cout<<"zg.zgclasses.hpp.zgRegisterOperators.cp5,idx2="<<idx2<<"\n";
  }
  zgRegisterOperators(int i) {
  	idx2 = i;
    cout<<"zg.zgclasses.hpp.zgRegisterOperators.cp1,idx2="<<idx2<<"\n";
    
  };
  zgRegisterOperators(std::vector<int> zlist){
  	idx2=0;
  	for(int & each: zlist){
  		idx2+= each;
  	}
  	cout<<"zg.zgclasses.hpp.zgRegisterOperators.cp4,idx2="<<idx2<<"\n";
  }
private:
    int idx2;
};

}
}
}
