#include <iostream>
#include "bintree.hpp"
#include "stack.hpp"
using namespace std;
// template class BinTreeNode<int>;
int main(int argc, char * argv[]){
    BinTreeNode<int> *mytree = new BinTreeNode<int>(0) ;
    mytree->CreateSampleTree();
    mytree->Traverse(mytree,cout);
    cout<<endl;
    // Stack<int>::StackTest();
    // cout<<endl;
    mytree->TraverseStack(mytree,cout);
    cout<<endl;
    return(0);
}