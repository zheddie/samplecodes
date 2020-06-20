/*
 * mrdbIfsCfg.H
 *
 *  Created on: Mar 8, 2016
 *      Author: zhanggan
 */

#ifndef MRDBIFSCFG_H_
#define MRDBIFSCFG_H_


#include "IASPUserSpaceEntry.H"
#include "IASPUserSpaceHeader.H"
#include "UserSpace.H"
namespace std{
typedef UserSpace<IASPUserSpaceHeader,IASPUserSpaceEntry> IASPUserSpace_t;


/*
 *
 *
QmrdbAddIfsiAsp
		To add one iASP into the db2m configuration.
		It would store the “<iASP Name>, <affinity System Name>” pair.
QmrdbRmvIfsiAsp
		To remov	e one iASP-affinity pair.
QmrdbChgIfsiAsp
		This could be used when do switchover for the affinity system change
		We do not support iASP name change
QmrdbRtvIfsiAsp
		To retrieve all the iASP pairs available.
 *
 */
extern "C" int mrdbAddIfsiAsp(MrdbiASPName_t *iasp,MrdbSystemName_t * sysname);
extern "C" int mrdbRmvIfsiAsp(MrdbiASPName_t *iasp,MrdbSystemName_t * sysname);
extern "C" int mredbChgIfsiasp(MrdbiASPName_t *iasp,MrdbSystemName_t * sysname);
extern "C" void * mrdbRtvIfsiAsp();

}
#endif /* MRDBIFSCFG_H_ */
