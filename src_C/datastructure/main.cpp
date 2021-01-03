#include <iostream>
#include <stdlib.h>
#include "string.hpp"
#include "arraystack.hpp"
#include "linkstack.hpp"

using namespace std;
void test_substring(){
//Tying to test the  substring operator
    String *zs1 = new String("Hello Gan.I am Gavin. Nice to meet you.");
    String zs2 = (*zs1)(5,8);
    zs2.printAll();    
}
void test_kmp(){
//Trying to test the KMP.
    String zs6("abaabbaba");
    ifstream f("generatedstring2.txt");
    String zs7(f);
    f.close();
    zs7.fastFindAll(zs6);
}
void test_arraystack(){
    ArrayStack<int> as(20);
    as.Push(3);
    as.Push(8);
    as.Push(12);
    as.Push(7);
    for(int i =0;i<4;i++){
        cout<<as.Pop()<<" ";
    }
    cout<<endl;
}
void test_linkstack(){
    //Gavin. This is not accepted by compiler, as it treat is a function, 
    //which returned LinkStack<int>.
    // LinkStack<int> as();

    LinkStack<int> as;

    as.Push(3);
    as.Push(8);
    as.Push(12);
    as.Push(7);
    for(int i =0;i<4;i++){
        cout<<as.Pop()<<" ";
    }
    cout<<endl;
}
int main(int argc,char * argv[]){
    //test_substring();
    //test_kmp();
    test_arraystack();
    test_linkstack();
    //test_linkstack();
    return(0);
}