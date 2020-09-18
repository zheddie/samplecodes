//============================================================================
//
// Name: MrdbLocalEngineData.C
//
// Module Name: MRDBLOCENG
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
// $A0            v7r4m0  160317 SBSMITH  New
//                                                                
//============================================================================
#include "MrdbLocalEngineData.H"
#include "MrdbUsrSpcDef.H"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <except.h>

MrdbLocalEngineData::MrdbLocalEngineData(_SPCPTR usrSpcPtr)
{
    localData = (MrdbLocalUserSpace_t *) usrSpcPtr;
    criticalData = (MrdbLocalCriticalData_t *) localData;
}

MrdbLocalEngineData::~MrdbLocalEngineData()
{
    
}

inline uint8 MrdbLocalEngineData::getState()
{
    return criticalData->state;
}

void MrdbLocalEngineData::setState(uint8 newState)
{
    criticalData->state = newState;
}

inline uint8 MrdbLocalEngineData::getCopyId()
{
    return criticalData->copyId;
}

void MrdbLocalEngineData::setCopyId(char newCopyId)
{
    criticalData->copyId;
}

inline uint32 MrdbLocalEngineData::getRolChangeCount()
{
    return criticalData->rolChangeCount;
}

inline void MrdbLocalEngineData::incrementRolChangeCount()
{
    (criticalData->rolChangeCount)++;
}

inline _SYSPTR MrdbLocalEngineData::getRolPtr()
{
    return criticalData->rolPtr;
}

void MrdbLocalEngineData::setRolPtr(_SYSPTR newPtr)
{
    criticalData->rolPtr = newPtr;
}


