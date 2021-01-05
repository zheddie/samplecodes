#ifndef _ZG_DATALISTSORT_H
#define _ZG_DATALISTSORT_H
const int DefaultSize = 100;
template <class Type> class datalist;
template <class Type> class Element{
private:
    Type key;
    //field otherdata;
public:
    Element(Type item ){key=item;}
    Element(){key=0;}
    Type getKey(){return key;}
    void setKey( Type x){key = x;}
    Element <Type > & operator = (Element <Type> & x) {this->key = x.key;}
    int operator ==(Type & x) {return !(this->key <x.key ||x.key<this->key);}
    int operator !=(Type & x) {return (this->key<x.key||x.key<this->key);}
    int operator <=(Type & x) {return !(this->key>x.key);}
    int operator >=(Type & x) {return !(this->key<x.key);}
    int operator <(Type & x) {return this->key>x.key;}
    friend ostream & operator <<(ostream & os,Element <Type> & el){
        os<<el.key;
        return os;
    }
};
template <class Type> class datalist{
public:
    datalist(int MaxSz = DefaultSize):MaxSize(MaxSz),CurrentSize(0){
        Vector = new Element<Type>[MaxSz];
    }
    void swap(Element<Type> & x,Element<Type> & y){
        Element <Type> temp = x;x = y ; y = temp;
    }
    int getCurrentSize()const {return CurrentSize;}
    Element <Type > * getVector()const {return Vector;}
    void AddData(Type item) {
        if(CurrentSize<MaxSize){
            Element <Type > *el = new Element<Type> (item);
            Vector[CurrentSize++] = *el;
        }else{
            cout<<"Vector full!"<<endl;
            exit(1);
        }
    }

    void printAll(){
        for(int i = 0 ;i<CurrentSize ;i++){
            cout << Vector[i];
        }
        cout<<endl;
    }
private:
    Element <Type > * Vector;
    int MaxSize,CurrentSize;
};
template <class Type> void doInsert(datalist<Type> & list,int i){
    Element <Type> temp = list.getVector()[i]; 
    int j=i;
    while (j>0 && temp.getKey()<list.getVector()[j-1].getKey()){
        list.getVector()[j] = list.getVector()[j-1];
        j--;
    }
    list.getVector()[j] = temp;
}
template <class Type> void InsertSort(datalist<Type> & list){
    for(int i = 1;i<list.getCurrentSize();i++){
        doInsert(list,i);
    }
}
#endif