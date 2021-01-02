#ifndef _ZG_STRING_H
#define _ZG_STRING_H
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
const int maxLen = 100*1000*1000;
class String{
public:
    String(const String & ob);
    String(const char * init);
    String( std::ifstream &fn) ;
    String();
    ~String(){delete [] ch;}
    int Length() const{return curLen;}
    String & operator()(int pos,int len);
    int operator == (const String & ob) const{return(strcmp(ch,ob.ch) == 0);}
    int operator != (const String & ob) const {return(strcmp(ch,ob.ch) != 0);}
    int operator ! ()const {return(curLen==0);}
    String & operator = (const String & ob);
    String & operator += (const String & ob);
    String & operator + (const String & ob);
    char operator [](int i);
    int Find(String & pat) const;
    int fastFind(String & pat) const;
    void fastFindAll(String & pat) const;
    void printfailArray()const;
private:
    int curLen;
    char * ch;
    int * f;
    void failArray();
};
#endif