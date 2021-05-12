#ifndef _ZG_TREE_ITERATOR_H
#define _ZG_TREE_ITERATOR_H
#include "bintree.hpp"
template <class Type>
class TreeIterator{
public:
    TreeIterator( BinaryTree<Type> &BT):T(BT),current(BT.GetRoot()){}
    virtual ~TreeIterator(){}
    virtual void First() =0;
    virtual void operator++() = 0;
    int operator+() const {return (current != NULL);}
    Type & operator()() const;
protected:
    BinaryTree <Type> &T;
    BinTreeNode<Type> * current;
};
template <class Type> Type& TreeIterator<Type>::operator()() const{
    if(current == NULL ){
        // cerr << "Illegal access" << endl;
        exit(1);
    }
    return current->GetData();
}
template <class Type>
struct StkNode{
    BinTreeNode<Type> * Node;
    int PopTim;
    StkNode( BinTreeNode<Type> * N = NULL):Node(N),PopTim(0){}
};
template <class Type> class PostOrder: public TreeIterator<Type>{
public:
    PostOrder( BinaryTree<Type> & BT);
    ~PostOrder(){}
    void First();
    void operator ++();
protected:
    LinkStack<StkNode<Type>> st;
};
template <class Type > PostOrder<Type>::PostOrder( BinaryTree<Type> & BT):TreeIterator<Type>(BT){
    //st.Push((StkNode<Type>)((this->T.GetRoot())));
}
template <class Type> void PostOrder<Type>::First(){
    st.MakeEmpty();
    if(this->T.GetRoot() != NULL) {
        st.Push(StkNode<Type> (this->T.GetRoot()));
        operator++();
        //(*this)++;
    }
}
template <class Type> void PostOrder<Type>::operator++(){
    // cout <<"in++..."<<endl;
    if(st.IsEmpty()){
        if(this->current == NULL) {
            cerr<<"Advanced past end"<< endl;exit(1);
        }
        this->current = NULL; return;
    }
    StkNode<Type> Cnode;
    for(;;){
        Cnode = *st.Pop();
        //cout <<"in++:"<< Cnode.Node->GetData()<<endl;
        if(++Cnode.PopTim == 3){
            this->current = Cnode.Node;return;
        }
        st.Push(Cnode);
        if(Cnode.PopTim == 1){
            if(Cnode.Node->GetLeft()!= NULL){
                st.Push(StkNode<Type>(Cnode.Node->GetLeft()));
            }
        }
        else{
            if(Cnode.Node->GetRight() != NULL)
                st.Push(StkNode<Type>(Cnode.Node->GetRight()));
        }
    }
}
#endif
