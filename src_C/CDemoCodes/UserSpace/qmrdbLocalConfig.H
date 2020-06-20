#ifndef QMRDBLOCALCONFIG_H
#define QMRDBLOCALCONFIG_H
//============================================================================
//
// Name: qmrdbLocalConfig.H
//
//
// Description: 
//============================================================================
//                                              
// Change Activity:                                           
//                                                         
// FLAG REASON    RELEASE DATE   PGMR     COMMENTS             
// ---- --------- ------- ------ -------- -----------------------------------
// $A0            v7r4m0  160222 SBSMITH  New
//                                                                
//============================================================================
#include "MrdbUsrSpcDef.H"

extern "C" int createLocalUserSpace();

extern "C" int destroyLocalUserSpace();

extern "C" int createSharedUserSpace();

extern "C" int destroySharedUserSpace();

int createUserSpace(const char * usrSpcName);

int destroyUserSpace(const char * usrSpcName);

extern "C" int initializeLocalUserSpace();

extern "C" int initializeSharedUserSpace();

#endif
