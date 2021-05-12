#ifndef _ZG_ARRAY_H
#define _ZG_ARRAY_H
#include <iostream>
#include <stdlib.h>
#define DEFAULT_SIZE 3
template <class Type> class Array{
    Type *elements;
    int ArraySize;
    void getArray();
public:
    Array(int Size=DEFAULT_SIZE);
    Array(const Array<Type>  & x);
    ~Array(){delete []elements};
    Array<Type> & operator = (const Array<Type> & A);
    Type & operator [] (int i);
    Type * operator *() const{return elements;};
    int Length() const {return ArraySize;};
    void ReSize(int sz);
};
template <class Type> void Array<Type>::getArray(){
    elements  = new Type[ArraySize];
    if( 0 == elements ){ cerr<<"Memory allocation failure"<<endl;ArraySize=0;return;}
}
template <class Type> Array <Type>::Array(int sz){
    if(sz<=0){
        cerr<<"Invalid array size input"<<endl;
        ArraySize = 0;
        return;
    }
    ArraySize = sz;
    getArray();
}
template <class Type> Array<Type>::Array(const Array<Type> & x){
    ArraySize = x.ArraySize;
    int n = ArraySize;
    if(ArraySize > 0){
        getArray();
        Type * srcp = x.elements;
        Type * tgtp = elements;
        while(n--){
            *tgtp++ = *srcp++;
        }
    }
}

template <class Type> Type & Array<Type>::operator [](int i){
    if(i<0 || i> ArraySize-1){
        cerr<<"Index out of range"<<endl;
        return;
    }
    return elements[i];
}

template <class Type> void Array<Type>::ReSize(int sz){
    if(sz <= 0){
        cerr<<"Invalid array size input"<<endl;
        return;
    }
    if(sz != ArraySize){
        Type * newarray = new Type[sz];
        if( 0 == newarray ){ cerr<<"Memory allocation failure"<<endl;return;}
        int n = (sz<=ArraySize)?sz:ArraySize;
        Type * srcp = elements;
        Type * tgtp = newarray;
        while(n--) *tgtp++=*srcp++;
        delete [] elements;
        elements = newarray;ArraySize = sz;
    }
    return;
}
#endif