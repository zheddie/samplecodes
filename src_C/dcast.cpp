/*
 * dcast.cpp
 *
 *  Created on: Dec 30, 2016
 *      Author: zhanggan
 */
#include <iostream>
using namespace std;
class Interface
{
public:
        virtual void GenericOp() = 0;// pure virtual function
        uint32_t age;
};

class SpecificClass : public Interface
{
public:
        virtual void GenericOp(){cout<<"In GenericOp"<<endl;};
        virtual void SpecificOp(){cout<<"In SpecificOp"<<endl;};
        uint8_t f1;
};
class SpecificClass2 : public Interface
{
public:
        virtual void GenericOp(){cout<<"In GenericOp2"<<endl;};
        virtual void SpecificOp(){cout<<"In SpecificOp2"<<endl;};
        uint64_t f2;
};
int main(){
	Interface * ptr_interface = new SpecificClass2();
	SpecificClass* ptr_specific = dynamic_cast<SpecificClass*>(ptr_interface);
	cout<<"ptr_interface:"<<ptr_interface<<",ptr_specific:"<<ptr_specific<<endl;
	if( ptr_specific ){
	    // our suspicions are confirmed -- it really was a SpecificClass
	    ptr_specific->SpecificOp();
	}else{
	    // our suspicions were incorrect -- it is definitely not a SpecificClass.
	    // The ptr_interface points to an instance of some other child class of the base InterfaceClass.
	    ptr_interface->GenericOp();
	};
	SpecificClass2* ptr_specific2 = dynamic_cast<SpecificClass2*>(ptr_interface);
	cout<<"ptr_interface:"<<ptr_interface<<",ptr_specific2:"<<ptr_specific2<<endl;
	if( ptr_specific2 ){
	    // our suspicions are confirmed -- it really was a SpecificClass
	    ptr_specific2->SpecificOp();
	}else{
	    // our suspicions were incorrect -- it is definitely not a SpecificClass.
	    // The ptr_interface points to an instance of some other child class of the base InterfaceClass.
	    ptr_interface->GenericOp();
	};
}
