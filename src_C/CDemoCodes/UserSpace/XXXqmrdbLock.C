//============================================================================
//
// Name: qmrdbLock.C
//
// Module Name: QMRDBLOCK
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

#include <except.h>
#include <licall.h>
#include <locksl.cleinc>
#include <unlocksl.cleinc>
#include <lock.cleinc>
#include <unlock.cleinc>

#include "qmrdbLock.H"
#include "mrdbTypes.H"

MrdbSpaceLock::MrdbSpaceLock(_SPCPTR * spcPtr, char lockState):
xSpcPtr(spcPtr),
xLockState(lockState)
{
    if (1==lock())
    {
	xSpcPtr = NULL;
    }
}

MrdbSpaceLock::~MrdbSpaceLock()
{
    if (xSpcPtr != NULL)
    {
	unlock();
	xSpcPtr = NULL;
    }
}

int MrdbSpaceLock::lock()
{
#pragma exception_handler(unexpected, 0, _C1_ALL, _C2_ALL, _CTLA_HANDLE)
    locksl( *xSpcPtr, xLockState );
# pragma disable_handler
    return 0;                         // Space was locked successfully

    unexpected:
      return 1;                       // Space was not locked
}

int MrdbSpaceLock::unlock()
{
#pragma exception_handler(unexpected, 0, _C1_ALL, _C2_ALL, _CTLA_HANDLE)
    unlocksl( *xSpcPtr, xLockState );
# pragma disable_handler
    return 0;                         // Space was unlocked successfully

    unexpected:
      return 1;                       // Space lock was not released
}


MrdbObjectLock::MrdbObjectLock(_SYSPTR * objPtr, char lockState):
xObjPtr(objPtr),
xLockState(lockState)
{
    if (1==lock())
    {
	xObjPtr = NULL;
    }
}

MrdbObjectLock::~MrdbObjectLock()
{
    if(xObjPtr != NULL)
    {
	unlock();
	xObjPtr = NULL;
    }
}

int MrdbObjectLock::lock()
{
    _LOCK_Template_T  lockTemplate;
    memset(&lockTemplate, 0x00, sizeof(lockTemplate));

    lockTemplate.Num_Requests = 1;
    // @TODO decide on request type
    lockTemplate.Request_Type = _LOCK_SYNCH | _LOCK_WAIT_ENTER_MOD | _LOCK_WAIT_LEAVE_MOD | _LOCK_WAIT_INDEFINITE;
    lockTemplate.Request_Options = _LOCK_THREAD_SCOPE;
    lockTemplate.Object[0] = *xObjPtr;
    lockTemplate.Lock_State = xLockState | _LOCK_ENTRY_ACTIVE;
    lockTemplate.Offset_Selections = offsetof( _LOCK_Template_T, Lock_State );

    // Handle any exceptions while the object is being locked
#pragma exception_handler(unexpected, 0, _C1_ALL, _C2_ALL, _CTLA_HANDLE)
    _LOCK(&lockTemplate);
#pragma disable_handler

    return 0;                         // Object was locked successfully

    unexpected:
      return 1;                       // Object was not locked
}

int MrdbObjectLock::unlock()
{
    _LOCK_Template_T unlockTemplate;
    memset(&unlockTemplate, 0x00, sizeof(unlockTemplate));

    unlockTemplate.Num_Requests = 1;
    unlockTemplate.Object[0] = *xObjPtr;
    unlockTemplate.Lock_State = xLockState | _LOCK_ENTRY_ACTIVE;
    unlockTemplate.Request_Options = _LOCK_THREAD_SCOPE;
    unlockTemplate.Offset_Selections = offsetof( _LOCK_Template_T, Lock_State );

    // Handle any exceptions while the object is being unlocked
#pragma exception_handler(unexpected, 0, _C1_ALL, _C2_ALL, _CTLA_HANDLE)
    _UNLOCK(&unlockTemplate);
#pragma disable_handler

    return 0;                         // Object was unlocked successfully

    unexpected:
      return 1;                       // Object lock was not released 
}

