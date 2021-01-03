#ifndef _ZG_ARRAYSTACK_H
#define _ZG_ARRAYSTACK_H

#include <iostream>
#include <assert.h>
#include "stackbase.hpp"

template <class Type> class ArrayStack:public StackBase<Type>{
public:
    ArrayStack(int=10);
    ~ ArrayStack(){ delete elements;}
    void Push(const Type & item);
    Type Pop();
    Type GetTop();
    void MakeEmpty(){ top = -1;this->currentSize=0;}
    int IsEmpty() const { return top == -1;}
    int IsFull() const { return top == maxSize - 1;}
private:
    int top;
    Type * elements;
    int maxSize;
};
template <class Type> ArrayStack<Type>::ArrayStack(int s):StackBase<Type>(), top(-1),maxSize(s){
    elements = new Type[maxSize];
    assert(elements!=NULL);
}
template <class Type> void  ArrayStack<Type>::Push(const Type & item){
    assert(!IsFull());
    elements[++top] = item;
    this->currentSize++;
}
template <class Type> Type  ArrayStack<Type>::Pop(){
    assert(!IsEmpty());
    this->currentSize--;
    return elements[top--];
}
template <class Type> Type  ArrayStack<Type>::GetTop(){
    assert(!IsEmpty());
    return elements[top];
}

#endif