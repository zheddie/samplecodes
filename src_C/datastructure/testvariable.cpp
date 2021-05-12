#include <iostream>
using namespace std;
template <class Type> class Base {
public:
    virtual void func(int s)=0;
   protected :
   int num = 7;
   Type data;
};
template <class Type> class Derived : public Base<Type> {
   public :
    void func(int s) {
      cout << "The value of num is: " << this->num<<",s="<<s<<endl;
   }
};

int main() {
   Derived<int>  obj;
   obj.func(2);
   int x = 10;
   int s = 80;
   int &z=x;
   const int &y = x;
   cout<<"x["<<&x<<"]="<<x<<",y["<<&y<<"]="<<y<<endl;
   return 0;
}