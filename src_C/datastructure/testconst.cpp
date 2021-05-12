#include <iostream>
using namespace std;
class A{
public:
    A(int i):x(i){}
    int  getint(int s) const  {
        return(x);
    }
private:
    int x;
};
class B{
public:
    B():b(0){}
    B(int i):b(i){}
    int operator()(){
        return(b+1);
    }
    int getValue(){return b;}
private:
    int b;
};
int main()
{
    B b1(8);
    B b2;
    cout<<b1()<<b2()<<endl;

    // int y = 8;
    // int & x =y;

    // // int c = 8;
    // // A a(8);
    // // //const int &y = a.getint(3);
    // // int y = a.getint(3);
    // // cout <<"y="<<y<<endl;
    
    // // cout <<"y="<<y<<endl;
    // // cout <<"x="<<a.getint(2)<<endl;

    // // const int data = 10;
    // // int &rData = data; // rData bound to data
    // // cout <<"data = " << data << endl;
    // // cout << "rData = " << rData << endl;

    // // const A * c = new A(5);
    // // A * e = (A*)c;
    // A* const c = new A(7); 
    // A* b = c;
    // cout <<b-> getint(2)<<endl;
    return 0;
}