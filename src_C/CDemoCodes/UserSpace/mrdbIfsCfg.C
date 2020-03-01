/*
 * mbrdIfsCfg.C
 *
 *  Created on: Mar 8, 2016
 *      Author: zhanggan
 */
#include "mrdbIfsCfg.H"
#define US_NAME "MRDBIFSCFG"
#define US_LIBRARY "QUSRSYS   "
namespace std{
extern "C" int mrdbAddIfsiAsp(MrdbiASPName_t *iasp,MrdbSystemName_t * sysname){

	class IASPUserSpaceEntry * entry = new IASPUserSpaceEntry(iasp,sysname);
	MrdbObjectName_t obj;
	MrdbLibraryName_t lib;
	memcpy(&obj,(void *)US_NAME,sizeof(MrdbObjectName_t));
	memcpy(&lib,(void *)US_LIBRARY,sizeof(MrdbLibraryName_t));
	cout<<"zgCP1"<<endl;
	IASPUserSpace_t * us = new IASPUserSpace_t(&obj,&lib);
	int rt = us->addOneEntry(*entry);
	cout<<"zgCP4"<<endl;
	return rt;
}
extern "C" int mrdbRmvIfsiAsp(MrdbiASPName_t *iasp,MrdbSystemName_t * sysname){

	class IASPUserSpaceEntry * entry = new IASPUserSpaceEntry(iasp,sysname);
	MrdbObjectName_t obj;
	MrdbLibraryName_t lib;
	memcpy(&obj,(void *)US_NAME,sizeof(MrdbObjectName_t));
	memcpy(&lib,(void *)US_LIBRARY,sizeof(MrdbLibraryName_t));

	IASPUserSpace_t * us = new IASPUserSpace_t(&obj,&lib);
	int rt = us->rmvOneEntry(*entry);
	return rt;
}

extern "C" int mrdbChgIfsiAsp(MrdbiASPName_t *iasp,MrdbSystemName_t * sysname){

	class IASPUserSpaceEntry * entry = new IASPUserSpaceEntry(iasp,sysname);
	MrdbObjectName_t obj;
	MrdbLibraryName_t lib;
	memcpy(&obj,(void *)US_NAME,sizeof(MrdbObjectName_t));
	memcpy(&lib,(void *)US_LIBRARY,sizeof(MrdbLibraryName_t));

	IASPUserSpace_t * us = new IASPUserSpace_t(&obj,&lib);
	int rt = us->chgOneEntry(*entry);
	return rt;
}

}
