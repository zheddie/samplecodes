/*
 * ZhengFangXing.cpp
 *
 *  Created on: Dec 30, 2016
 *      Author: zhanggan
 */

#include "ZhengFangXing.h"

namespace std {

ZhengFangXing::ZhengFangXing(double bc){
	BianChang = bc;
}

ZhengFangXing::~ZhengFangXing() {
	// TODO Auto-generated destructor stub
}
double ZhengFangXing::MianJi(){
	return BianChang * BianChang;
}
} /* namespace std */
