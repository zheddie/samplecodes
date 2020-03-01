/*
 * IASPUserSpaceEntry.h
 *
 *  Created on: Mar 7, 2016
 *      Author: zhanggan
 */
#include "UserSpace.H"
#include "UserSpaceEntry.H"

#ifndef IASPUSERSPACEENTRY_H_
#define IASPUSERSPACEENTRY_H_

namespace std {

class IASPUserSpaceEntry:public UserSpaceEntry {
public:
	IASPUserSpaceEntry();
	IASPUserSpaceEntry(const MrdbiASPName_t *,const MrdbSystemName_t *);

	virtual ~IASPUserSpaceEntry();
	int  operator==( IASPUserSpaceEntry &);
	char * getiASPName() const ;
	void printMe();
private:
	MrdbiASPName_t iASPName;
	char reserved1[16-sizeof(MrdbiASPName_t)];
	MrdbSystemName_t AffinitySystem;
	char reserved2[16-sizeof(MrdbSystemName_t)];
};

} /* namespace std */

#endif /* IASPUSERSPACEENTRY_H_ */
