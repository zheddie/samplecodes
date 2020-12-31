#ifndef _STACK_H
#define _STACK_H
#include <iostream>
#include <memory.h>
#define STACK_PARAM_ERROR -1
#define STACK_FULL -2
template <class Type>
class Stack{
public:
    Stack(int mx){ 
        maxlen=mx>=0?mx:0;
        if(maxlen>0){
            stack = (Type **)malloc(sizeof(Type *)*maxlen);
            memset(stack,0,sizeof(Type*)*maxlen);
        }
        sp = stack-1;
        };
    Type * pop(){
        if(sp>=stack){
            return(*(sp--));
        }else{
            return(NULL);
        }
    }
    int push(Type *item){
        if (item != NULL){
            if(sp-stack <maxlen-1){
                sp++;
                *sp = item;
                // std::cout<<"*item="<<*item<<std::endl;
            }else //stack full.
            {
                return(STACK_FULL);
            }
        }else{
            return(STACK_PARAM_ERROR);
        }
    }
    static int StackTest(){
        Stack<int> mystk(20);
        int * pint =(int *)malloc(sizeof(int)*5);
        for(int i=0;i<5;i++){
            pint[i]=i;
        }
        //std::cout<<"pint*="<<pint[0]<<pint[1]<<std::endl;
        mystk.push(&pint[0]);
        mystk.push(&pint[1]);
        mystk.push(&pint[2]);
        std::cout<<*mystk.pop()<<' ';
        std::cout<<*mystk.pop()<<' ';
        mystk.push(&pint[3]);
        mystk.push(&pint[4]);
        std::cout<<*mystk.pop()<<' ';
        std::cout<<*mystk.pop()<<' ';
        std::cout<<*mystk.pop()<<' ';
    }
    
    
private:
    int maxlen;
    Type ** sp;
    Type ** stack;
};

#endif