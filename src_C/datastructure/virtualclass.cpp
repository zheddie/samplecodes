#include <iostream>
using namespace std;
class ClassA{
public:
    virtual void print(){
        cout<<"This is Class A"<<endl;
    }
};
class ClassB:public ClassA{
public:
    void print(){
        cout<<"This is Class B"<<endl;
    }
};
int main(){
    ClassA ca;
    ClassB cb;
    ca.print();
    cb.print();
    ClassA *pa=&ca;
    ClassA *pb=&cb;
    pa->print();
    pb->print();
    return(0);
}