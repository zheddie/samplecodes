#include <iostream>
#include <ostream>
using namespace std;
template <class Type> class MBase {
public:
   MBase(int a):i(a){}
   MBase(){}
template <class U> friend std::ostream & operator<< (std::ostream & os,MBase<U> & t);
private: 
   int i;
};
template <class Type> std::ostream & operator<< (std::ostream & os,MBase<Type> & t){
   os<<t.i<<endl;
   return(os);
}
// class IBase {
// public:
//    IBase(int a):i(a){}
// friend std::ostream & operator<< (std::ostream & os,IBase & t);
// private: 
//    int i;
// };

// std::ostream & operator<< (std::ostream & os,IBase & t){
//    os<<t.i<<endl;
//    return(os);
// }
int main() {
   MBase<int> bb(5);
   cout<<bb<<endl;
   cout << "-1/2:="<<-1/2<<endl;
   // IBase b2(8);
   // cout<<b2<<endl;
   return 0;
}