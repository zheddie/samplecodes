/*
 * IASPUserSpaceHeader.h
 *
 *  Created on: Mar 7, 2016
 *      Author: zhanggan
 */

#ifndef IASPUSERSPACEHEADER_H_
#define IASPUSERSPACEHEADER_H_

#include "UserSpaceHeader.H"
#include "UserSpace.H"
namespace std {

class IASPUserSpaceHeader: public UserSpaceHeader {
public:
	IASPUserSpaceHeader();
	IASPUserSpaceHeader(MrdbRlsName_t *,MrdbPTFName_t *);

	virtual ~IASPUserSpaceHeader();

private:

	MrdbRlsName_t rlsInfo;
	char reserved1[16-sizeof(MrdbRlsName_t)];
	MrdbPTFName_t ptfInfo;
	char reserved2[32-sizeof(MrdbPTFName_t)];
};

} /* namespace std */

#endif /* IASPUSERSPACEHEADER_H_ */
