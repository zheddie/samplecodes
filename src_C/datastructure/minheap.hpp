#ifndef _ZG_MINHEAP_H
#define _ZG_MINHEAP_H
#include <iostream>
#include <ostream>
template <class Type> class MinPQ{
public:
    virtual int Insert(const Type & ) = 0;
    virtual Type RemoveMin() = 0;
};
template <class Type> class MinHeap: public MinPQ <Type>{
public:
    MinHeap(int maxSize);
    MinHeap(Type arr[],int n);
    ~MinHeap(){delete [] heap;}
    const MinHeap<Type> & operator=(const MinHeap & R);
    int Insert(const Type & x){
        if(CurrentSize == MaxHeapSize) {
            Type * newheap = new Type[2*MaxHeapSize];
            for(int i =0;i<CurrentSize;i++){
                newheap[i] = heap[i];
            }
            delete [] heap;
            heap = newheap;
            MaxHeapSize = 2*MaxHeapSize;
            cerr<< "Heap extended to "<<MaxHeapSize<<endl;
        }
        heap[CurrentSize] = x;
        FilterUp2(CurrentSize);
        CurrentSize++;
    };
    Type  RemoveMin(){
        if(!CurrentSize){cerr<<"Heap Empty"<<endl;return 0;}
        Type x = heap[0];
        heap[0]=heap[CurrentSize -1];
        CurrentSize --;
        FilterDown(0,CurrentSize - 1);
        return x;
    };
    int isEmpty() const {return CurrentSize == 0;}
    int isFull() const {return CurrentSize == MaxHeapSize;}
    void MakeEmpty() {CurrentSize = 0;}
    int getCurrentSize() const {return CurrentSize;}
    template <class T>friend ostream & operator << (ostream & os,MinHeap<T> & t);
private:
    enum{DefaultSize= 10};
    Type * heap;
    int CurrentSize;
    int MaxHeapSize;
    void FilterDown(int i , int m);
    void FilterUp(int i);
    void FilterUp2(int i);
};
template <class Type> ostream & operator << (ostream & os,MinHeap<Type> & t){
    for(int i =0;i<t.CurrentSize;i++){
        os<<t.heap[i]<<" ";
    }
    return os;
}
template <class Type>  MinHeap<Type> ::MinHeap(int maxSize){
    MaxHeapSize = DefaultSize<maxSize? maxSize:DefaultSize;
    heap = new Type[MaxHeapSize];
    CurrentSize = 0;
}
template <class Type> MinHeap<Type>::MinHeap(Type arr[],int n){
    MaxHeapSize = DefaultSize < n ? n:DefaultSize;
    heap = new Type[MaxHeapSize];
    memcpy(heap,arr,sizeof(Type)*MaxHeapSize);
    CurrentSize = n;
    int currentPos = (CurrentSize -2)/2;
    while(currentPos >=0){
        FilterDown(currentPos,CurrentSize - 1);
        currentPos--;
    }
}
template < class Type> void MinHeap<Type>::FilterDown(const int start,const int EndOfHeap){
    int i = start,j = 2*i+1;Type temp = heap[i];
    while(j<=EndOfHeap){
        if(j<EndOfHeap && heap[j] > heap[j+1]) j++;
        if(temp <= heap[j]) break;
        else{
            heap[i]= heap[j];
            i = j;j=2*i+1;
        }
    }
    heap[i] = temp;
}
template < class Type> void MinHeap<Type>::FilterUp(const int start){
    if(start > 0){
        int parent = (start-1)/2;
        Type temp =heap[start];
        if(heap[parent] >heap[start]){
            heap[start] = heap[parent];
            heap[parent] = temp;
            FilterUp(parent);
        }
    }
    return;
}
template < class Type> void MinHeap<Type>::FilterUp2(const int start){
    int j = start;
    int parent = (j-1)/2;
    Type temp =heap[j];
    while(j>0){
        // cout<<"cp0,j="<<j<<",parent="<<parent<<endl;
        if(heap[parent] <=temp) break;
        else{
            heap[j] = heap[parent];
            j = parent;
            parent = (j-1)/2;
            // cout<<"cp1,j="<<j<<",parent="<<parent<<endl;
        }
    }
    heap[j]=temp;
}
#endif