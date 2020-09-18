//============================================================================
//
// Name: MrdbSharedEngineData.C
//
// Module Name: MRDBSHRDTA
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
#include "MrdbSharedEngineData.H"
#include "MrdbUsrSpcDef.H"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <except.h>

MrdbSharedEngineData::MrdbSharedEngineData(_SPCPTR usrSpcPtr)
{
    sharedData = (MrdbSharedUserSpace_t *) usrSpcPtr;
    criticalData = (MrdbSharedCriticalData_t *) sharedData;
    ifsData = (MrdbSharedIfsData_t *) sharedData->ifsConfigPtr;
}

MrdbSharedEngineData::~MrdbSharedEngineData()
{
    
}

