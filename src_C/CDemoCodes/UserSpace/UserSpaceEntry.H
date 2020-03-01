/*
 * UserSpaceEntry.H
 *
 *  Created on: Mar 15, 2016
 *      Author: zhanggan
 */

#ifndef USERSPACEENTRY_H_
#define USERSPACEENTRY_H_

namespace std {

class UserSpaceEntry {
public:
	UserSpaceEntry();
	virtual ~UserSpaceEntry();
	virtual int  operator==( UserSpaceEntry &) =0;
};

} /* namespace std */

#endif /* USERSPACEENTRY_H_ */
