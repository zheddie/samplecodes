#ifndef _ZG_STACKBASE_H
#define _ZG_STACKBASE_H
#include <iostream>

template <class Type> class StackBase{
public:
    StackBase():currentSize(0){}
    virtual ~StackBase(){};
    virtual void Push(const Type & item) = 0;
    virtual Type Pop() = 0;
    virtual Type GetTop() =0;
    virtual void MakeEmpty() = 0;
    virtual int IsEmpty() const =0;
    virtual int IsFull() const =0;
    int getSize() const {return currentSize;}
protected:
    int currentSize;
};

#endif