#ifndef _BIN_TREE_H
#define _BIN_TREE_H
#include <iostream>
#include <istream>
#include <ostream>

#include "linkstack.hpp"
using namespace std;
template <class Type>
class BinTreeNode{
public:
    BinTreeNode():data(-1),leftChild(NULL),rightChild(NULL){};
    BinTreeNode(Type item,BinTreeNode<Type> *left=NULL,BinTreeNode<Type> *right=NULL):data(item),leftChild(left),rightChild(right){};
    Type & GetData() {return(data);}
    BinTreeNode<Type> * GetLeft()const {return leftChild;}
    BinTreeNode<Type> * GetRight()const {return rightChild;}
    void  SetLeft( BinTreeNode<Type> * l) {leftChild = l;}
    void SetRight( BinTreeNode<Type> * r) {rightChild = r;}
    
private:
    Type data;
    BinTreeNode<Type> *leftChild,*rightChild;
};
template <class Type> class BinaryTree{
    template <class T> friend istream & operator >>(istream & in, BinaryTree<T> & Tree);
    template <class T> friend ostream & operator <<(ostream & out,BinaryTree<T> & Tree);
public:
    BinaryTree():root(NULL){}
    BinaryTree(const BinTreeNode<Type> & rt):root((BinTreeNode<Type>*)&rt){}
    BinaryTree(Type value):RefValue(value),root(NULL){}
    virtual ~BinaryTree(){destroy(root);}
    virtual int IsEmpty() {return root == NULL?1:0;}
    virtual BinTreeNode<Type> * Parent(BinTreeNode<Type> * current){
        return root == NULL || root==current ? NULL:Parent(root,current);
    }
    virtual BinTreeNode<Type> * LeftChild(BinTreeNode <Type> * current){
        return root != NULL? current->GetLeft():NULL;
    }
    virtual BinTreeNode<Type> * RightChild(BinTreeNode <Type> * current){
        return root != NULL? current->GetRight():NULL;
    }
    virtual int Insert(const Type & item);
    virtual int Find(const Type & item) const;
    BinTreeNode < Type > * GetRoot()  { return root;}
    //friend void CreateSimpleTree(const BinTreeNode<Type> & root);
    void Traverse(BinTreeNode<Type> * current , ostream & out) const;
private:
    BinTreeNode<Type> * root;
    Type RefValue;
    BinTreeNode<Type> *Parent(BinTreeNode<Type> * start, BinTreeNode<Type> * current);
    int Insert(BinTreeNode<Type> * & current, const Type & item);
    
    int Find(BinTreeNode<Type> * current , const Type & item ) const;
    void destroy(BinTreeNode<Type> * current);
};
template <class Type> BinaryTree<Type> * CreateSimpleTree(BinTreeNode<Type> & root){
    BinTreeNode<Type> * p5 = new BinTreeNode<Type> (5);
    BinTreeNode<Type> * p4 = new BinTreeNode<Type>(4,NULL,p5);
    BinTreeNode<Type> * p3 = new BinTreeNode<Type>(3);
    BinTreeNode<Type> * p2 = new BinTreeNode<Type>(2,p4);
    BinTreeNode<Type> * p1 = new BinTreeNode<Type>(1,p3);
    root.SetLeft(p1);
    root.SetRight(p2);
    BinaryTree<Type> *pbt = new  BinaryTree<Type> (root);
    return pbt;
}
template <class Type> void BinaryTree<Type>::destroy(BinTreeNode<Type> * current){
    if(current != NULL){
        destroy(current->GetLeft());
        destroy(current->GetRight());
        delete current;
    }
}
template <class Type > BinTreeNode<Type> * BinaryTree<Type>::Parent(BinTreeNode<Type> * start,BinTreeNode<Type> * current){
    if(start == NULL) return NULL;
    if(start->GetLeft() == current || start->GetRight() == current) return start;
    BinTreeNode <Type> * p ;
    if((p = Parent(start->GetLeft(), current )) != NULL) return p;
    else return Parent(start->GetRight(),current);
}
template<class Type> void BinaryTree<Type>::Traverse(BinTreeNode<Type> * current , ostream &out)const {
    if(current != NULL){
        Traverse(current->GetLeft(),out);
        out<<current -> GetData()<<' ';
        Traverse(current-> GetRight(),out);
    }
}
template<class Type> int BinaryTree<Type>::Insert(const Type & item) {
    return -1;
}
template<class Type> int BinaryTree<Type>::Find(const Type & item)const {
    return -1;
}
template<class Type> istream & operator>>(istream & in , BinaryTree<Type> & Tree){
    Type item;
    cout<< "construct binary tree:\n";
    cout<<"Input data(end with "<<Tree.RefValue<< "):";
    in >> item;
    while ( item != Tree.RefValue){
        Tree.Insert(item);
        cout<< "Input data(end with "<< Tree.RefValue<<"):";
        in >> item;

    }
    return in ;
}
template <class Type> ostream & operator << (ostream & out, BinaryTree<Type> & Tree){
    out << "Preorder traversal of binary tree. \n";
    Tree.Traverse(Tree.root,out);
    out<<endl;
    return out;
}


#endif