/*
 * Sample.h
 *
 *  Created on: Mar 5, 2016
 *      Author: zhanggan
 */

#ifndef SAMPLE_H_
#define SAMPLE_H_

namespace std {
template <typename H>
class Sample {
public:
	Sample(){};
	virtual ~Sample(){};
	int getHsize(){ return sizeof(H);}
private:
	H header;
};

} /* namespace std */

#endif /* SAMPLE_H_ */
