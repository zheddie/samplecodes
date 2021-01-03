#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <memory.h>
#include <string.h>
#include <chrono> //For the nano time .
#include "string.hpp"
using namespace std;
String::String(const String & ob):f(NULL){
    ch = new char[maxLen+1];
    if(!ch){cerr<<"Allocation failure"<<endl;exit(1);}
    curLen = ob.curLen;
    strcpy(ch,ob.ch);
}
String::String( ifstream &fn):f(NULL) {
    ch = new char[maxLen+1];
    if(!ch){cerr<<"Allocation failure"<<endl;exit(1);}
    memset(ch,0,maxLen+1);
    int i =0;
    char c;
    do{
        c=fn.get();
        ch[i++]=c;
    }while(c!='\n');
    ch[i-1]='\0';
    curLen = i;
}
String::String(const char * init):f(NULL){
    ch = new char[maxLen+1];
    if(!ch){cerr<<"Allocation failure"<<endl;exit(1);}
    curLen = strlen(init);
    strcpy(ch,init);
}
String::String():f(NULL){
    ch = new char[maxLen+1];
    if(!ch){cerr<<"Allocation failure"<<endl;exit(1);}
    curLen = 0;
    ch[0]='\0';
}
String & String::operator()(int pos,int len){
    String * temp = new String;
    if(pos< 0 || pos+len-1 >= maxLen || len < 0 ){
        temp->curLen=0;temp->ch[0] = '\0';
    }else{
        if(pos+len-1>=curLen) len = curLen - pos;
        temp->curLen = len;
        for(int i = 0,j=pos;i<len;i++,j++) temp->ch[i] = ch[j];
        temp->ch[len] = '\0';
    }
    return(*temp);
}
String & String ::operator = (const String & ob){
    if(&ob!=this){
        delete [] ch;
        ch = new char[maxLen+1];
        if(!ch){cerr<<"Allocation failure"<<endl;exit(1);}
        curLen = ob.curLen;
        strcpy(ch,ob.ch);
    }else{
        cerr<<"Attempted assignment of a string to itself!"<<endl;
    }
    return *this;
}
String & String::operator +=(const String & ob){
    char * temp = ch;
    curLen +=ob.curLen;
    ch = new char[maxLen+1];
    if(!ch){cerr <<"Out of Memory!"<<endl;exit(1);}
    strcpy(ch,temp);
    strcat(ch,ob.ch);
    delete [] temp;
    return * this;
}
String & String::operator +(const String & ob){
    char * temp = ch;
    curLen +=ob.curLen;
    ch = new char[maxLen+1];
    if(!ch){cerr <<"Out of Memory!"<<endl;exit(1);}
    strcpy(ch,temp);
    strcat(ch,ob.ch);
    delete [] temp;
    return * this;
}
char   String::operator [](int i){
    if(i < 0 && i>= curLen){cout<<"Out of Boundary!"<<endl;exit(1);}
    return ch[i];
}
int String::Find(String & pat) const{
    char * p = pat.ch, *s = ch; int i = 0;
    if(*p && *s){
        while(i<=curLen-pat.curLen){
            cout<<"*p="<<*p<<",*s="<<*s<<",i="<<i<<endl;
            if(*p++ == *s++){
                cout<<"match"<<endl;
                if(!*p) return(i);
            }else{
                cout<<"NOmatch"<<endl;
                i++;s=ch+i;p=pat.ch;
            }
        }
    }
    return(-1);
}
void String::printAll()const{
    if(ch){
        cout<<ch<<endl;
    }
}
void String::printfailArray()const{
    if(f){
        for(int i =0;i<curLen;i++){
            cout<<f[i]<<" ";
        }
        cout<<endl;
    }
}
/**
 * @brief Building the failure function for the patent string.
 * 
 */
void String::failArray(){
    int lenP = curLen;
    if(!f){
        f = new int[maxLen+1];
        if(!f){cerr<<"Allocation failure"<<endl;exit(1);}
    }
    memset(f,0,maxLen+1);
    f[0] = -1;
    for(int j=1;j<lenP;j++){
        int i = f[j-1];
        while(*(ch+j) != *(ch+i+1) && i>=0) i=f[i];
        if(*(ch+j) == *(ch+i+1)) f[j] = i+1;
        else f[j] = -1;
        cout<<f[j]<<' ';
    }
}
/**
 * @brief Fast find with KMP .
 * 
 * @param pat 
 * @return int 
 */
int String::fastFind(String & pat) const{
    int posP = 0, posT = 0;
    int lenP = pat.curLen, lenT = curLen;
    pat.failArray();
    while(posP < lenP && posT < lenT){
        cout<<"pat["<<posP<<"]"<<pat[posP]<<",ch["<<posT<<"]"<<ch[posT]<<endl;
        if(pat.ch[posP] == ch[posT]){
            posP++; posT++;
        }else if (posP == 0 ) {
            posT++;
        }else{
            posP = pat.f[posP-1]+1;
        }
    }
    if(posP < lenP) return -1;
    else return (posT-lenP);
}

void String::fastFindAll(String & pat) const{
    int posP = 0, posT = 0;
    int lenP = pat.curLen, lenT = curLen;
    pat.failArray();
    cout<<endl;
    int count =0;
    auto start = chrono::high_resolution_clock::now();

    while(posT<lenT){
        while(posP < lenP && posT < lenT){
            //cout<<"pat["<<posP<<"]"<<pat[posP]<<",ch["<<posT<<"]"<<ch[posT]<<endl;
            if(pat.ch[posP] == ch[posT]){
                posP++; posT++;
            }else if (posP == 0 ) {
                posT++;
            }else{
                posP = pat.f[posP-1]+1;
                //Gavin. Following code is trying to reduce the unnecessary 
                // compare if we already found the compare is the same compare. 
                // BUT, looks like it's this introduce more compare between numbers
                // and the test indicate that this kind of "reduce" didn't get any 
                // better performance , in fact, it has worse performane.
                // if(pat.f[posP-1]+1 == pat.f[posP]) {
                //     while(pat.f[posP-1]+1 == pat.f[posP]){
                //         posP = pat.f[posP-1]+1;
                //         if(posP == 0){ posT++; }
                //     }
                // }else{
                //     posP = pat.f[posP-1]+1;
                // }
                
            }
        }
        if(posP < lenP) {
            break;
        }
        //else cout<<posT-lenP<<" ";
        count++;
        posP = 0;
    }
    auto finish = chrono::high_resolution_clock::now();
    cout<< "total="<<count<<",time:"<< chrono::duration_cast<chrono::nanoseconds>(finish-start).count() << "ns\n";
    return;
}