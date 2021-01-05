#include <iostream>
#include <stdlib.h>
#include "string.hpp"
#include "arraystack.hpp"
#include "linkstack.hpp"
#include "bintree.hpp"
#include "treeiterator.hpp"
#include "datalistsort.hpp"
using namespace std;
void test_substring(){
//Tying to test the  substring operator
    String *zs1 = new String("Hello Gan.I am Gavin. Nice to meet you.");
    String zs2 = (*zs1)(5,8);
    zs2.printAll();    
}
void test_kmp(){
//Trying to test the KMP.
    String zs6("abaabbaba");
    ifstream f("generatedstring2.txt");
    String zs7(f);
    f.close();
    zs7.fastFindAll(zs6);
}
void test_arraystack(){
    ArrayStack<int> as(20);
    as.Push(3);
    as.Push(8);
    as.Push(12);
    as.Push(7);
    for(int i =0;i<4;i++){
        cout<<*as.Pop()<<" ";
    }
    cout<<endl;
}
class pp{
    public:
    pp(int s,char t):i(s),n(t){}

    int i;
    char n;
};
void test_linkstack(){
    //Gavin. This is not accepted by compiler, as it treat is a function, 
    //which returned LinkStack<int>.
    // LinkStack<int> as();

    LinkStack<pp> ass;
    pp s1(3,'a');
    pp s2(8,'b');
    pp s3(12,'c');
    pp s4(7,'d');
    
    ass.Push(s1);
    ass.Push(s2);
    ass.Push(s3);
    ass.Push(s4);
    for(int i =0;i<4;i++){
        cout<<ass.Pop()->n<<" ";
    }
    cout<<endl;
}
void test_bintree(){
    BinTreeNode<int> rt(9);
    BinaryTree<int>  * pBinTree = CreateSimpleTree(rt);
    // pBinTree->Traverse(&rt,cout);
    PostOrder<int> poTree(*pBinTree);
    poTree.First();
    int v = 0;
    //for(int v=poTree.First();v=poTree(); ){
    while(1){//We depends on the exit in operator() .
        v=poTree();
        cout <<"in test:"<<v <<endl;
        poTree.operator++();
    }
    cout<<endl;
}
datalist<int> * CreateSampleDatalist(){
    datalist<int> *dl = new datalist<int>(50);
    dl->AddData(8);
    dl->AddData(2);
    dl->AddData(4);
    dl->AddData(5);
    dl->AddData(3);
    dl->AddData(1);
    dl->AddData(7);
    dl->AddData(9);
    dl->AddData(6);
    return dl;
}
void test_sort(){
    datalist<int> * dl = CreateSampleDatalist();
    dl->printAll();
    InsertSort(*dl);
    dl->printAll();
}
int main(int argc,char * argv[]){
    //test_substring();
    //test_kmp();
    // test_arraystack();
    // test_linkstack();
    //test_linkstack();
    //test_bintree();
    test_sort();
    return(0);
}