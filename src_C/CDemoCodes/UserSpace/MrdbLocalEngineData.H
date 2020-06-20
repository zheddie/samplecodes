#ifndef MRDBLOCALENGINEDATA_H
#define MRDBLOCALENGINEDATA_H
//============================================================================
//
// Name: MrdbLocalEngineData.H
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

class MrdbLocalEngineData
{
  public:
    MrdbLocalEngineData(_SPCPTR usrSpcPtr);
    ~MrdbLocalEngineData();
    uint8 getState();
    void setState(uint8 newState);
    uint8 getCopyId();
    uint32 getRolChangeCount();
    void incrementRolChangeCount();
    _SYSPTR getRolPtr();
    void setRolPtr(_SYSPTR newPtr);
    

  private:
    // Prevent anyone from updating the copy id
    void setCopyId(char newCopyId);

    MrdbLocalUserSpace_t * localData;
    MrdbLocalCriticalData_t * criticalData;
    
};

#endif
