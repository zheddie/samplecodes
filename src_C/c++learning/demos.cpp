#include <iostream>

using namespace std;
struct address{
    char * name;
    long int number;
    char * street;
    char state1[2];
    char * town;
    char state2[2];
    char state3[3];
    long zip;
};
void checkstructsize(){
    address add;
    cout <<"sizeofaddress:"<<sizeof(struct address)<<endl;
    cout <<"sizeofadd:"<<sizeof(add)<<endl;
    cout <<"&add="<<(void *)&add<<",&name="<<(void *)&(add.name)
        <<",&number="<<(void *)&(add.number)
        <<",&street="<<(void *)&(add.street)
        <<",&state1="<<(void *)&(add.state1[0])
        <<",&town="<<(void *)&(add.town)
        <<",&state2="<<(void *)&(add.state2[0])
        <<",&state3="<<(void *)&(add.state3[0])
        <<",&zip="<<(void *)&(add.zip)<<endl;
}
void f4(){
    int a=1;
    const int c = 2;
    const int * p1 = &c;
    const int * p2 = &a;
    cout<<"c:"<<c<<endl;
    cout<<"p1:"<<(void *)p1<<":"<<*p1<<endl;
    int * p3 = &a;
    cout<<"p3:"<<(void *)p3<<":"<<*p3<<endl;
    p3--;
    *p3=8;
    cout<<"c:"<<c<<endl;
    cout<<"p3:"<<(void *)p3<<":"<<*p3<<endl;
    cout<<"p1:"<<(void *)p1<<":"<<*p1<<endl;

    // int * p3 = &c;
    // *p3 = 7;
}
void f1(){
    char p[] = {'h','e','l','l','o','\0'};
    char q[] = "hello";
    char pp[] = "hello";
    cout<<(void *)q<<":"<<q<<endl<<(void *)p<<":"<<p<<endl<<(void *)pp<<":"<<pp<<endl;
    //p[2]='s';
    pp[2]='s';
    cout<<p<<endl<<pp<<endl;
}
int main(){
    // f1();
    //f4();
    checkstructsize();
}