#ifndef MRDBSHAREDENGINEDATA_H
#define MRDBSHAREDENGINEDATA_H
//============================================================================
//
// Name: MrdbSharedEngineData.H
//
//
// Description: 
//============================================================================
//                                              
// Change Activity:                                           
//                                                         
// FLAG REASON    RELEASE DATE   PGMR     COMMENTS             
// ---- --------- ------- ------ -------- -----------------------------------
// $A0            v7r4m0  160317 SBSMITH  New
//                                                                
//============================================================================
#include "MrdbUsrSpcDef.H"

class MrdbSharedEngineData
{
  public:
    MrdbSharedEngineData(_SPCPTR usrSpcPtr);
    ~MrdbSharedEngineData();

  private:

    MrdbSharedUserSpace_t *     sharedData;
    MrdbSharedCriticalData_t *  criticalData;
    MrdbSharedIfsConfig_t *     ifsData;
    
};

#endif
