#include <iostream>
#include <vector>
#include <string>

namespace torch {namespace jit{
    namespace {
using namespace std;

struct  zgOperator {
public:
  
  zgOperator(const std::string& schema, const std::string& lambda) {
    std::cout<<"zg.zgclasses.hpp.zgOperator.constructor(std::string&, std::string&),schema="<<schema<<",lambda="<<lambda<<"\n";
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
      std::cout<<"zg.zgclasses.hpp.zgRegisterOperators.constructor(std::vector<zgOperator> ),idx2="<<idx2++<<"\n";
    }
  }

private:
    int idx2;
};
struct zgOperatorSet {
  zgOperatorSet(std::initializer_list<const char *> sig_literals){
    for (const char * sig : sig_literals) {
      printf("zg.sig:%s\n",sig);
    }
  };

};
}
}
}
