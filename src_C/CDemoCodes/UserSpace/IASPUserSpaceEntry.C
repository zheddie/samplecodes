/*
 * IASPUserSpaceEntry.cpp
 *
 *  Created on: Mar 7, 2016
 *      Author: zhanggan
 */

#include "IASPUserSpaceEntry.H"
#include <assert.h>
#include <string.h>
namespace std {

IASPUserSpaceEntry::IASPUserSpaceEntry() {
	// TODO Auto-generated constructor stub

}
IASPUserSpaceEntry::IASPUserSpaceEntry(const MrdbiASPName_t *iasp,const MrdbSystemName_t *sysname){
	assert(iasp!=NULL);assert(sysname!=NULL);
	memcpy(iASPName,iasp,sizeof(MrdbiASPName_t));
	memcpy(AffinitySystem,sysname,sizeof(MrdbSystemName_t));

}
char  * IASPUserSpaceEntry::getiASPName() const {
	return (char  *)iASPName;
}
void   IASPUserSpaceEntry::printMe()  {
	cout<<"iASPName="<<iASPName<<",AffinitySys="<<AffinitySystem<<endl;
}
int IASPUserSpaceEntry::operator ==( IASPUserSpaceEntry & e2){
	return strncmp((char *)getiASPName(),(char *)e2.getiASPName(),sizeof(MrdbiASPName_t));
}

IASPUserSpaceEntry::~IASPUserSpaceEntry() {
	// TODO Auto-generated destructor stub
}

} /* namespace std */
