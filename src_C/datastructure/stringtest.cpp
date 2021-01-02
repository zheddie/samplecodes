#include <iostream>
#include <stdlib.h>
#include "string.hpp"
using namespace std;
int main(int argc,char * argv[]){
    // String *zs1 = new String("Hello Gan.I am Gavin. Nice to meet you.");
    // String *zs2 = new String("World!");
    // String zs4("Gavin");
    // String zs3(*zs1+*zs2);
    // char s = zs3[7];
    // int pos = zs1->Find(zs4);
    // cout<<"pos:7="<<s<<",ll pos:"<<pos<<endl;
    String *zs5 = new String("ttabaabcabbttabaabcabattabaabcabatt");
    String zs6("abaabbaba");
    String zs9("ttabababuvwabababatt");
    String zs8("abababa");
    zs9.fastFindAll(zs8);
    // int pos = zs5->fastFind(zs6);
    // cout<<"pos:"<<pos<<endl;
    // zs5->fastFindAll(zs8);
    ifstream f("generatedstring2.txt");
    String zs7(f);
    f.close();
    zs7.fastFindAll(zs6);

    
    return(0);
}