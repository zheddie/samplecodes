#ifndef _BIN_TREE_H
#define _BIN_TREE_H
#include <iostream>
#include "stack.hpp"
enum NodeState{
    NS_NOT_ACCESSED,
    NS_ACCESSED
};
template <class Type>
class BinTreeNode{
public:
    BinTreeNode():data(-1),flag(NS_NOT_ACCESSED),leftChild(NULL),rightChild(NULL){};
    BinTreeNode(Type item,BinTreeNode<Type> *left=NULL,BinTreeNode<Type> *right=NULL):data(item),flag(NS_NOT_ACCESSED),leftChild(left),rightChild(right){};
    void CreateSampleTree();
    void Traverse(BinTreeNode<Type> * , std::ostream & )const;
    void TraverseStack(BinTreeNode<Type> * , std::ostream & )const;
    bool isBalanced(BinTreeNode<Type> *) const;
    BinTreeNode<Type> * processOneNode(Stack<BinTreeNode<Type>> *pstk, std::ostream & out);
private:
    Type data;
    NodeState flag;   //This flag is used to indicate whether this node is accessed already.
    BinTreeNode<Type> *leftChild,*rightChild;
};
template <class Type> BinTreeNode<Type> * checkOneNode(BinTreeNode<Type> *current , int depth){
    if(current->leftChild != NULL){

    }
}
bool handleDepth(int *ld,int depth){
    for(int i =0;i<2;i++){
        if(ld[i]!=0){
            if(abs(ld[i]-depth) >1){
                return (false);
            }else if(ld[i]-depth == 0){
                return(true);
            }
        }else{
            ld[i] = depth;
        }
    }
    return(true);
}
template <class Type> int getHeigth(BinTreeNode<Type> *current){
    if(current == NULL) return(0);
    int ld = getHeigth(current->leftChild);
    int rd = getHeigth(current->rightChild);
    return ld>rd?ld+1:rd+1;
    
}
template <class Type> bool BinTreeNode<Type>::isBalanced(BinTreeNode<Type> *current) const{
    int leafdepth [2] = {0,0,0};
    int ld = 0,rd=0;
    if(current== NULL) return(true);

    if(current->leftChild!= NULL){
        ld = getHeigth(current->leftChild);
    }
    if(current->rightChild!= NULL){
        rd = getHeigth(current->rightChild);
    }
    if(abs(ld-rd)>1){
        return(false);
    }
    return isBalanced(current->leftChild) && isBalanced(current->rightChild);
}
template <class Type> void BinTreeNode<Type>::Traverse(BinTreeNode<Type> *current , std::ostream & out) const{
    if(current != NULL){
        Traverse(current->leftChild,out);
        out << current->data << ' ';
        Traverse(current->rightChild,out);
    }
}
template <class Type> BinTreeNode<Type> * BinTreeNode<Type>::processOneNode(Stack<BinTreeNode<Type> > *pstk, std::ostream & out){
    if(this->leftChild != NULL && this->leftChild->flag != NS_ACCESSED){
        pstk->push(this);
        return(this->leftChild);
    }
    else{
        out << this->data << ' ';
        this->flag = NS_ACCESSED;
        if(this->rightChild != NULL && this->rightChild->flag != NS_NOT_ACCESSED){
            return(this->rightChild);
        }else{
            return(pstk->pop());
        }
    }
}
template <class Type> void BinTreeNode<Type>::TraverseStack(BinTreeNode<Type> *current , std::ostream & out) const{
    if(current != NULL){
        Stack<BinTreeNode<Type>> localStack(20);
        BinTreeNode<Type> * p =current;
        while(p != NULL){
            std::cout<<"test:p="<<p->data<<std::endl;
            p = p->processOneNode(&localStack,out);
        };
    }
}
template <class Type> void BinTreeNode<Type>::CreateSampleTree(){
    BinTreeNode<Type> * p5 = new BinTreeNode<Type> (5);
    BinTreeNode<Type> * p4 = new BinTreeNode<Type>(4,NULL,p5);
    BinTreeNode<Type> * p3 = new BinTreeNode<Type>(3);
    BinTreeNode<Type> * p2 = new BinTreeNode<Type>(2,p4);
    BinTreeNode<Type> * p1 = new BinTreeNode<Type>(1,p3);
    this->leftChild = p1;
    this->rightChild= p2;
}
#endif