#ifndef QMRDBUSRSPCDEF_H
#define QMRDBUSRSPCDEF_H
//============================================================================
//
// Name: MrdbUsrSpcDef.H
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
#include "mrdbTypes.H"

#define QMRDBLOCAL "QMRDBLOCAL"
#define QMRDBCOMON "QMRDBCOMON"
#define QMRDB      "QMRDB     "
#define QMRDBROLIX "QMRDBROLIX"

////////////////////////////////////////////////////////////////
// Local user space that contains unique config data per system
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// MrdbLocalCriticalData_t: Critical run-time data & pointers
// ****NOTE****
// When MrdbLocalCriticalData_t is changed, 
// the XXX part in SLIC also needs to be changed
/////////////////////////////////////////////////////////////////
typedef _Packed struct 
{
    // The state of DB2Mirror
    uint8   state;                                      // 0x00
    
    // copyId: Source or target of the flash copy.
    // This value will be set during the genesis creation
    // of the DB2Mirror environment when one system is  
    // flash copied to create the second system. 
    // This value SHOULD NOT CHANGE after genesis creation.
    // Possible values are:
    // 'S': The original "source" system
    // 'T': The copy "target" system
    char    copyId;                                     // 0x01 
	
    char    reserved1[2];                               // 0x02
    
    // Replicated object list (ROL) change count
    uint32  rolChangeCount;                             // 0x04
    
    // Align pointers on 16 byte boundary
    char    reserved2[8];                               // 0x08
    
    // System pointer to the replicated object list (ROL)
    _SYSPTR rolPtr;                                     // 0x10
    
    // System pointer to the engine message queue
    _SYSPTR engMsgQueuePtr;                             // 0x20
    
    // System pointer to the object replication 
    // manager (ORM) message queue
    _SYSPTR ormMsgQueuePtr;                             // 0x30
    
    // Tracked object list pointer
    _SYSPTR trackObjPtr;                                // 0x40
    
    // Reserved for future use
    char    reserved3[];                                // 0x50
} MrdbLocalCriticalData_t;

/////////////////////////////////////////////////////////////////
// TBD - unused
/////////////////////////////////////////////////////////////////
typedef _Packed struct
{
    // Reserved for future use
    char	reserved[4096];
} MrdbLocalSection1_t;

/////////////////////////////////////////////////////////////////
// Entire local user space layout
/////////////////////////////////////////////////////////////////
typedef _Packed struct
{
    // Section 0 Critical run-time data & pointers
    MrdbLocalCriticalData_t criticalData;               //0x0000
    char                    reserved1[1920];            //0x0080
    MrdbLocalSection1_t *   section1Ptr;                //0x0800
    char                    sectionPtrs[112];           //0x0810
    char                    reserved2[1920];            //0x0880

    // Section 1 TBD
    MrdbLocalSection1_t     section1;                   //0x1000

} MrdbLocalUserSpace_t;


/////////////////////////////////////////////////////////////////
// Shared user space that is replicated between the two systems
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// MrdbSharedCriticalData_t: Critical run-time data
/////////////////////////////////////////////////////////////////
typedef _Packed struct 
{
    char    reserved[128];
} MrdbSharedCriticalData_t;

/////////////////////////////////////////////////////////////////
// MrdbSharedIfsConfig_t: IFS config data
/////////////////////////////////////////////////////////////////
#define IASPMAX 223
typedef _Packed struct
{
    char    iaspName[10];
    char    systemAffinity[8];
    char    reserved[14];
} MrdbIfsEntry_t;

typedef _Packed struct
{
    uint32          iaspCount;                          //0x0000
    char            reserved[28];                       //0x0004
    MrdbIfsEntry_t  iaspTable[IASPMAX];                 //0x0020
    char            reserved2[1024];                    //0x1C00
} MrdbSharedIfsConfig_t;

/////////////////////////////////////////////////////////////////
// Entire shared user space layout
/////////////////////////////////////////////////////////////////
typedef _Packed struct
{
    // Section 0 Critical run-time data & pointers
    MrdbSharedCriticalData_t    criticalData;           //0x0000
    char                        reserved1[1920];        //0x0080
    MrdbSharedIfsConfig_t *     ifsConfigPtr;           //0x0800
    char                        sectionPtrs[112];       //0x0810
    char                        reserved2[1920];        //0x0880

    // Section 1 IFS config
    MrdbSharedIfsConfig_t       ifsConfig;              //0x1000

    char                        reserved3;              //0x3000

} MrdbSharedUserSpace_t;

#endif
