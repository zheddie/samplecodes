#ifndef _ZG_LINKSTACK_H
#define _ZG_LINKSTACK_H

#include <iostream>
#include <assert.h>
#include "stackbase.hpp"
template <class Type> class StackNode{
public:
    StackNode(const Type & item, StackNode<Type> * l = NULL):data(item),link(l){}
    Type data;
    StackNode<Type> * link;
};

template <class Type> class LinkStack:public StackBase<Type>{
public:
    LinkStack():top(NULL){}
    ~ LinkStack(){ 
        StackNode<Type>  * next = NULL;
        while (top != NULL){
            next = top->link;
            delete top;
            top = next;
        }
    }
    void Push(const Type & item);
    Type Pop();
    Type GetTop();
    void MakeEmpty(){ 
        StackNode<Type>  * next = NULL;
        while (top != NULL){
            next = top->link;
            delete top;
            top = next;
            
        }
        this->currentSize =0;
    }
    int IsEmpty() const { return top == NULL;}
    int IsFull() const { return false;}
private:
    StackNode<Type> *top;
};

template <class Type> void  LinkStack<Type>::Push(const Type & item){
    StackNode<Type> * n = new StackNode<Type>(item,top);
    assert(n != NULL);
    top = n;
}
template <class Type> Type  LinkStack<Type>::Pop(){
    if(top){
        StackNode<Type> * n = top;
        top = top->link;
        return n->data;
    }else{
        return (Type)NULL;
    }
}
template <class Type> Type  LinkStack<Type>::GetTop(){
    if(top){
        return top->data;
    }else{
        return (Type)NULL;
    }
}

#endif