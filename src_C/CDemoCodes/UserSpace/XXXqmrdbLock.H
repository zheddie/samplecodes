#ifndef QMRDBLOCK_H
#define QMRDBLOCK_H
//============================================================================
//
// Name: qmrdbLock.H
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
#include <pointer.h>

class MrdbSpaceLock
{
  public:
    MrdbSpaceLock(_SPCPTR * spcPtr, char lockState);
    ~MrdbSpaceLock();

  private:
    // Will return a zero if successful or a one if unsuccessful.
    int lock();
    int unlock();

    _SPCPTR * xSpcPtr;
    char      xLockState;
};

class MrdbObjectLock
{
  public:
    MrdbObjectLock(_SYSPTR * objPtr, char lockState);
    ~MrdbObjectLock();

  private:
    // Will return a zero if successful or a one if unsuccessful.
    int lock();
    int unlock();

    _SYSPTR * xObjPtr;
    char      xLockState;
};

#endif
