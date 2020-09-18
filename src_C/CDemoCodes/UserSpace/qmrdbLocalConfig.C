//============================================================================
//
// Name: qmrdbLocalConfig.C
//
// Module Name: QMRDBLOCFG
//
// Service Program Name:
// Program Name:
// State/Domain:
// Activation Group: 
//
// Description: 
//
//============================================================================
//                                              
// Change Activity:                                           
//                                                         
// FLAG REASON    RELEASE DATE   PGMR     COMMENTS             
// ---- --------- ------- ------ -------- -----------------------------------
// $A0            v7r4m0  160222 SBSMITH  New
//                                                                
//============================================================================
#include "qmrdbLocalConfig.H"
#include "MrdbUsrSpcDef.H"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <except.h>
#include <sysspace.cinc>
#include <rslvsp.h>
#include <setsppfp.cleinc>
//////////////////////////////////////////////////////////////////////////////
// Inline Static Function Definitions
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Function Definitions
//////////////////////////////////////////////////////////////////////////////


//============================================================================
//
// Name: createLocalUserSpace
//
// Purpose: This function creates the user space that contains the unique
//          local DB2Mirror environment data
//
// Parameters:    None
//
// Return Value: (MrdbReturnCode)
//        Success         - The user space was successfully created
//        UnexpectedError - An unknown exception was encountered
//
// Dependencies:  None
//
// Exceptions Signaled:  None
//
//============================================================================
extern "C" int QmrdbCreateLocalUserSpace()
{
    return createUserSpace(QMRDBLOCAL);
}

extern "C" int QmrdbCreateSharedUserSpace()
{
    return createUserSpace(QMRDBCOMON);
}

extern "C" int QmrdbDestroyLocalUserSpace()
{
    return destroyUserSpace(QMRDBLOCAL);
}

extern "C" int QmrdbDestroySharedUserSpace()
{
    return destroyUserSpace(QMRDBCOMON);
}

int createUserSpace(const char * usrSpcName)
{
    // CRTS template and template extension
    typedef _Packed struct CRTStmpl
    {
	_CRTS_Template_T      space;
	_CRTS_Ext_Template_T  extension;
    }   CRTStmpl_t;
    CRTStmpl_t            tmpl;

    // initialize template
    memset( &tmpl, 0x00, sizeof(tmpl) );
    memset( tmpl.space.Object.Name, ' ', 30 );

    // get the system pointers to QMRDB library and QSYS user profile
#pragma exception_handler(unexpected, 0, _C1_ALL, _C2_ALL, _CTLA_HANDLE)
    _SYSPTR qmrdbLibSyp = rslvsp(WLI_LIB,    QMRDB, "QSYS", _AUTH_NONE);
    _SYSPTR qsysUserSyp = rslvsp(WLI_USRPRF, "QSYS","QSYS", _AUTH_NONE);
#pragma disable_handler

    // set up CRTS template
    tmpl.space.TmpSize             = sizeof(tmpl);
    strncpy( tmpl.space.Object.Name, usrSpcName , 10 );
    tmpl.space.Object.Type_Subtype = _Usrspc;
    tmpl.space.Permanent           = 1;		// make space permanent
    tmpl.space.Var_Length          = 1;		// variable length
    tmpl.space.In_Context          = 1;		// insert in context
    tmpl.space.Owner               = 1;		// owner specified
    tmpl.space.Auto_Extend         = 1;		// auto extend
    tmpl.space.Protect_Lvl         = 3;		// no user read/modify
    tmpl.space.Size                = 65536;	// inital size
    tmpl.space.Perf_Class.MainMemory  = 1;	// in main memory at create time
    tmpl.space.Perf_Class.MchAlign    = 1;	// machine align on page bndry
    tmpl.space.Perf_Class.MainStgPool = 1;	// machine storage pool
    tmpl.space.Library           = qmrdbLibSyp;	// put in library QMRDB

    // extention template
    tmpl.space.Ext_Offset       = sizeof(tmpl) - sizeof(tmpl.extension);
    tmpl.extension.User_Profile = qsysUserSyp;
    __memcpy( tmpl.extension.Domain_Assigned, "\x80\x00", 2 ); //system domain

    _SYSPTR objSysPtr = NULL;
    _SPCPTR usrSpcPtr = NULL;

#pragma exception_handler(unexpected, 0, _C1_ALL, _C2_ALL, _CTLA_HANDLE)
    // create the new space and resolve a space 
    // pointer to the data inside the user space
    _CRTS( &objSysPtr, (_CRTS_Template_T *)&tmpl );
    usrSpcPtr = (char *)_SETSPPFP( objSysPtr );
#pragma disable_handler

    return 0;

    unexpected: return 1;
}

int destroyUserSpace(const char * usrSpcName)
{
    _SYSPTR objSysPtr = NULL;

#pragma exception_handler(unexpected, 0, _C1_ALL, _C2_ALL, _CTLA_HANDLE)
    objSysPtr = rslvsp(WLI_USRSPC, (char*) usrSpcName, QMRDB, _AUTH_NONE);
#pragma disable_handler

    if (objSysPtr != NULL)
    {
#pragma exception_handler(unexpected, 0, _C1_ALL, _C2_ALL, _CTLA_HANDLE)
	_DESS( &objSysPtr );
#pragma disable_handler
    }

    return 0;

    unexpected: return 1;
}


extern "C" int initializeLocalUserSpace()
{
    _SYSPTR objSysPtr = NULL;
    _SPCPTR usrSpcPtr = NULL;

#pragma exception_handler(unexpected, 0, _C1_ALL, _C2_ALL, _CTLA_HANDLE)
    objSysPtr = rslvsp(WLI_USRSPC, QMRDBLOCAL, QMRDB, _AUTH_NONE);
    usrSpcPtr = (char *)_SETSPPFP( objSysPtr );
#pragma disable_handler

    if (usrSpcPtr != NULL)
    {
	MrdbLocalUserSpace_t * engineData = (MrdbLocalUserSpace_t *) usrSpcPtr;
	memset(engineData, 0x00, sizeof(MrdbLocalUserSpace_t));

	engineData->criticalData.state = 1;
	engineData->criticalData.rolChangeCount = 1;
#pragma exception_handler(rolError, 0, _C1_ALL, _C2_ALL, _CTLA_HANDLE)
	_SYSPTR rolSysPtr = rslvsp(WLI_GENIDX, QMRDBROLIX, "QSYS", _AUTH_NONE);
	engineData->criticalData.rolPtr = rolSysPtr;
#pragma disable_handler
	rolError:

	engineData->criticalData.engMsgQueuePtr = NULL;
	engineData->criticalData.trackObjPtr = NULL;

	engineData->section1Ptr = &(engineData->section1);

	memset(&(engineData->section1), '1', sizeof(MrdbLocalSection1_t));
    }
    
    return 0;

    unexpected: return 1;
}

/*extern "C" MrdbReturnCode setPointersToLocalUserSpace()
{
    // @TODO Set pointer in WCBTH

    // @TODO Set pointer in SLIC (TBD where that is)
}*/

extern "C" int initializeSharedUserSpace()
{
    _SYSPTR objSysPtr = NULL;
    _SPCPTR usrSpcPtr = NULL;

#pragma exception_handler(unexpected, 0, _C1_ALL, _C2_ALL, _CTLA_HANDLE)
    objSysPtr = rslvsp(WLI_USRSPC, QMRDBCOMON, QMRDB, _AUTH_NONE);
    usrSpcPtr = (char *)_SETSPPFP( objSysPtr );
#pragma disable_handler

    if (usrSpcPtr != NULL)
    {
	MrdbSharedUserSpace_t * engineData = (MrdbSharedUserSpace_t *) usrSpcPtr;
	memset(engineData, 0x00, sizeof(MrdbSharedUserSpace_t));

	engineData->ifsConfigPtr = &(engineData->ifsConfig);

	memset(&(engineData->ifsConfig), 'I', sizeof(MrdbSharedIfsConfig_t));

	engineData->ifsConfig.iaspCount = 1;
	strncpy(engineData->ifsConfig.iaspTable[0].iaspName,"IASP000001", 10);
	strncpy(engineData->ifsConfig.iaspTable[0].systemAffinity, "SYSTEM01", 8);

	engineData->reserved3 = 'X';
    }

    return 0;

    unexpected: return 1;
    
}
