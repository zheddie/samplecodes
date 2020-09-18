/*
 * templateFun.cpp
 *
 *  Created on: Mar 5, 2016
 *      Author: zhanggan
 */

#include "templateFun.h"
#include <iostream>
namespace std {
//void zgFuc1(int a){
//	cout <<"zgFuc1="<<a<<endl;
//}
export template <typename S>  void zgsizeof(){
	cout<<"sizeof(S)="<<sizeof(S)<<endl;
}
template <typename S>  void zga(){
	cout<<"H="<<sizeof(S)*2<<endl;
}
} /* namespace std */
