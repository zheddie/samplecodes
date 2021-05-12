#ifndef _ZG_STACKBASE_H
#define _ZG_STACKBASE_H
#include <iostream>

template <class Type> class StackBase{
public:
    StackBase():currentSize(0){}
    virtual ~StackBase(){};
    virtual void Push( Type item) = 0;
    virtual Type * Pop() = 0;
    virtual Type * GetTop() =0;
    virtual void MakeEmpty() = 0;
    virtual int IsEmpty()  =0;
    virtual int IsFull()  =0;
    int getSize()  {return currentSize;}
protected:
    int currentSize;
};

#endif