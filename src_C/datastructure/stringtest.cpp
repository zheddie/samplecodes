#include <iostream>
#include <stdlib.h>
#include "string.hpp"
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
int main(int argc,char * argv[]){
    test_substring();
    //test_kmp();
    return(0);
}