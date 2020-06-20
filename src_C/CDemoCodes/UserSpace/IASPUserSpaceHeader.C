/*
 * IASPUserSpaceHeader.cpp
 *
 *  Created on: Mar 7, 2016
 *      Author: zhanggan
 */

#include "IASPUserSpaceHeader.H"

#include <string.h>
namespace std {

IASPUserSpaceHeader::IASPUserSpaceHeader() {
	// This construct is used to retrieve data from userspace object.

}
IASPUserSpaceHeader::IASPUserSpaceHeader(MrdbRlsName_t * rls,MrdbPTFName_t *ptf) {
	// TODO Auto-generated constructor stub
	if(rls != NULL){
		memcpy(rlsInfo,rls,sizeof(MrdbRlsName_t));
	}
	if(ptf !=NULL){
		memcpy(ptfInfo,ptf,sizeof(MrdbPTFName_t));
	}
	NumberOfEntries = 0;
}

IASPUserSpaceHeader::~IASPUserSpaceHeader() {
	// TODO Auto-generated destructor stub
}

} /* namespace std */
