/*
 * ZhengFangXing.h
 *
 *  Created on: Dec 30, 2016
 *      Author: zhanggan
 */

#ifndef ZHENGFANGXING_H_
#define ZHENGFANGXING_H_

#include "DuoBianXing.h"

class ZhengFangXing : public DuoBianXing {
public:
	ZhengFangXing(double );
	virtual ~ZhengFangXing();
	virtual double MianJI();
private:
	double BianChang;
};

#endif /* ZHENGFANGXING_H_ */
