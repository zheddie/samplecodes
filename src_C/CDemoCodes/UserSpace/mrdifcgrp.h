/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ---------------------------------------------------------------- */
/* IBM Confidential                                                 */
/*                                                                  */
/* OCO Source Materials                                             */
/*                                                                  */
/* Product(s):                                                      */
/*     5770-SS1                                                     */
/*                                                                  */
/* (C)Copyright IBM Corp.  2016, 2018                               */
/*                                                                  */
/* The Source code for this program is not published  or otherwise  */
/* divested of its trade secrets,  irrespective of what has been    */
/* deposited with the U.S. Copyright Office.                        */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
#ifndef _MRDIFCGRP_H
#define _MRDIFCGRP_H

#pragma datamodel(P128)

#ifdef __ILEC400__
#define QMRDIFCGRP_EXTERN
#else
#define QMRDIFCGRP_EXTERN extern "C"
#endif

/**********************************************************************/
/*                                                                    */
/* DESCRIPTIVE NAME = Engine Configuration SPIs for                   */
/*                    network Interface Group Information             */
/*                    Definitions and Functions                       */
/*                                                                    */
/* FUNCTION =     This header file defines constants, data types,     */
/*                and SPI function prototypes for                     */
/*                Engine Configuration SPIs for Interface Group.      */
/*                                                                    */
/* Change Activity:                                                   */
/* Flg Reason    Version YYYYMMDD Userid   Description                */
/* ___ _________ _______ ________ ________ __________________________ */
/* $A0           7.4     20160127 changle  New include and prototype. */
/*                                                                    */
/**********************************************************************/

/**********************************************************************/
/* SPI Result codes.  These will be set in the "result" field of      */
/* the  tocIfcGrpResult_t structure.                                  */
/**********************************************************************/
#include<tocifcgrp.h> /* for struct tocIfcGrpIPAddrPair               */
/* The following reason codes may be returned for all SPI functions:  */
#define MRDIFCGRP_componentNameLen 15    /* The component name length */
#define MRDIFCGRP_descriptionLen 50 /* The mapping description length */


#define MRDIFCGRP_OK                       0 /* Successful API call   */
#define MRDIFCGRP_VfyptrsError             6 /* Pointer verification  */
#define MRDIFCGRP_ERROR                9999  /* Unexpected error      */
#define MRDIFCGRP_GroupNotFound           30 /* Group not found,      */
                                             /* this should be warning*/ 
                                             /* for this SPI          */ 
#define MRDIFCGRP_GroupWrongState 31 /* Group is in wrong state */
#define MRDIFCGRP_ComponentNotFound 40 /* Component not found */ 
#define MRDIFCGRP_NoSpcAuth 5 /* No *IOSYSCFG authority*/
#define MRDIFCGRP_InvalidComponentName 41 /* Invalid component name */
#define MRDIFCGRP_DupComponentName     42 /* Duplicate component name */
#define MRDIFCGRP_GroupERROR 32 /* Group operation error, need check further error code from TOC component */
#define MRDIFCGRP_ComponentNameNotFound 43 /* Cannot find the component name*/

#define SPACEOBJNAME "QMRDIFGCFG"
#define IFGCFG_USER_SPACE_NAME SPACEOBJNAME "QUSRSYS   "
#define MRDIFCGRP_MaxMappingEntries 32


/**********************************************************************/
/* Type definitions                                                   */
/**********************************************************************/
/* Store the description of the name mapping                          */
typedef char mrdIfcGrpDescription[MRDIFCGRP_descriptionLen];  
/* Component name                                                     */
typedef char mrdIfcGrpComponentName[MRDIFCGRP_componentNameLen];  

typedef char tocIfcGrpName[GroupNameLen];  

typedef _Packed struct {
    int result;              /* Result of SPI call                    */
    int additionalErrorCode; /* Additional error code                 */
    int offset;              /* Offset of error                       */
    char reserved[52]; 	     /* Reserved for future use               */
} mrdIfcGrpSPIResult_t;      /* Store the returned error code         */

/* This is used for input of adding interface group for DB2Mirror     */
typedef _Packed struct {
    mrdIfcGrpComponentName componentName;           /* Component Name */
    char reserverd1;                         /* Reserved for alignment*/
    int pairCount;                          /* Count of Address Pairs */
    tocIfcGrpIPAddrPair ifcPairs[TOCIFCGRP_MaxPairs];/* Address pairs */
} mrdIfcGrp_t;

typedef _Packed struct {
    mrdIfcGrpComponentName componentName;           /* Component Name */
    char reserverd1;                         /* Reserved for alignment*/
    tocIfcGrpName groupName;                   /* Interface group name*/
    mrdIfcGrpDescription groupNameInfoDesc;   /*Description of the map*/
    char reserverd2[2];                      /* Reserved for alignment*/
} mrdIfgNameMappingInfo_t;

typedef _Packed struct{
	char rlsInfo[7];
	char ptfinfo[8];
	char reserved1[1];
	int Offset_List_Data;
	int Number_List_Entries;
	int Size_Each_Entry;
	int initialized;
	char reserved2[32];

}mrdIfgMapspaceHeader_t;

/**********************************************************************/
/* SPI Function Prototypes                                            */
/**********************************************************************/
/**************************************************************/
/* Method:  QmrdAddIfgMap                                     */
/* Purpose:  Adds the name mapping of component and           */
/*           interface group name in to the user space.       */
/*           This SPI does nothing related to networking      */
/*           interface group configuration.                   */
/*           It only operates on the mapping information.     */
/* Parameters:  Pointer to mrdIfgNameMappingInfo_t  (Input)   */
/*              Pointer to mrdIfcGrpSPIResult_t (Input/Output)*/
/* Return value: None                                         */
/* Pre-conditions:  None                                      */
/* Post-conditions: If successful, a vaild interface group    */
/*                  and the component name mapping information*/
/*                  will be written to the QUSRSYS network    */
/*                  information user space object.            */
/*                  If an error occurred, the structure       */
/*                  mrdIfcGrpSPIResult_t will contain details */
/*                  about the error.                          */
/**************************************************************/
QMRDIFCGRP_EXTERN void QmrdAddIfgMap(mrdIfgNameMappingInfo_t *, 
		mrdIfcGrpSPIResult_t *);

/**************************************************************/
/* Method:  QmrdRmvIfgMap                                     */
/* Purpose:  Delete the name mapping of component and         */
/*           interface group name in to the user space.       */
/*           This SPI does nothing related to networking      */
/*           interface group configuration.                   */
/*           It only operates on the mapping information.     */
/* Parameters:  Pointer to mrdIfgNameMappingInfo_t  (Input)   */
/*              Pointer to mrdIfcGrpSPIResult_t (Input/Output)*/
/* Return value: None                                         */
/* Pre-conditions:  None                                      */
/* Post-conditions: If successful, the interface group        */
/*                  and the component name mapping information*/
/*                  will be deleted from the QUSRSYS network  */
/*                  information user space object.            */
/*                  If an error occurred, the structure       */
/*                  mrdIfcGrpSPIResult_t will contain details */
/*                  about the error.                          */
/**************************************************************/
QMRDIFCGRP_EXTERN void QmrdRmvIfgMap(mrdIfcGrpComponentName, 
		mrdIfcGrpSPIResult_t *);

/**************************************************************/
/* Method:  QmrdRtvIfgMap                                     */
/* Purpose:  retrieves the mapping between component name and */
/*           interface group name from the user space         */
/*           into mrdIfgNameMappingInfo_t.                    */
/* Parameters:  String for mrdIfcGrpComponentName   (Input)   */
/*              Pointer to mrdIfgNameMappingInfo_t  (Onput)   */
/*              Pointer to mrdIfcGrpSPIResult_t (Input/Output)*/
/* Return value: None                                         */
/* Pre-conditions:  None                                      */
/* Post-conditions: If successful, the interface group        */
/*                  and the component name mapping information*/
/*                  will be retrieved from the QUSRSYS network*/
/*                  information user space object and write to*/
/*                  mrdIfgNameMappingInfo_t                   */
/*                  If an error occurred, the structure       */
/*                  mrdIfcGrpSPIResult_t will contain details */
/*                  about the error.                          */
/**************************************************************/
QMRDIFCGRP_EXTERN void QmrdRtvIfgMap(mrdIfcGrpComponentName, 
		mrdIfgNameMappingInfo_t *, 
		mrdIfcGrpSPIResult_t *);

/**************************************************************/
/* Method:  QmrdAddIfcGrp                                     */
/* Purpose:  Adds the specified interface group for           */
/*           the particular component. It will store the      */
/*           mapping between component name and               */
/*           networking interface group name into repository. */
/*           And it will create the networking interface group*/
/*           by the given IP pairs and generated group name.  */
/* Parameters:                                                */
/*              Pointer to mrdIfcGrp_t  (Input)               */
/*              Pointer to mrdIfcGrpSPIResult_t (Input/Output)*/
/* Return value: None                                         */
/* Pre-conditions:  None                                      */
/* Post-conditions: If successful, a vaild interface group    */
/*                  and the component name mapping information*/
/*                  will be written to the QUSRSYS network    */
/*                  information user space object.            */
 /*                 And a valid interface group               */
/*                  will be written to the QUSRSYS interface  */
/*                  group file.                               */
/*                  If an error occurred, the structure       */
/*                  mrdIfcGrpSPIResult_t will contain details */
/*                  about the error.                          */
/**************************************************************/
QMRDIFCGRP_EXTERN void QmrdAddIfcGrp(mrdIfcGrp_t *, 
		mrdIfcGrpSPIResult_t *);

/**************************************************************/
/* Method:  QmrdRmvIfcGrp                                     */
/* Purpose:  Deletes the related interface group according to */
/*            component name from the system's TCP/IP         */ 
/*	     configuration, and deletes the component         */ 
/*	     name/interface group name mapping information    */ 
/*	     from Engine configuration.                       */
/* Parameters:                                                */
/*              mrdIfcGrpComponentName  (Input)               */
/*              Pointer to mrdIfcGrpSPIResult_t (Input/Output)*/
/* Return value: None                                         */
/* Pre-conditions:  None                                      */
/* Post-conditions: If successful, the interface group        */
/*                  and the component name mapping information*/
/*                  will be deleted from the QUSRSYS network  */
/*                  information user space object.            */
 /*                 And the interface group                   */
/*                  will be deleted from the QUSRSYS interface*/
/*                  group file also.                          */
/*                  If an error occurred, the structure       */
/*                  mrdIfcGrpSPIResult_t will contain details */
/*                  about the error.                          */
/**************************************************************/
QMRDIFCGRP_EXTERN void QmrdRmvIfcGrp(mrdIfcGrpComponentName, 
		mrdIfcGrpSPIResult_t *);

QMRDIFCGRP_EXTERN void QmrdAddIfgMapExt(mrdIfgNameMappingInfo_t * info, 
		int replace,
		mrdIfcGrpSPIResult_t * ret);

QMRDIFCGRP_EXTERN void QmrdRtvAllIfgMaps(mrdIfgNameMappingInfo_t infoArray[MRDIFCGRP_MaxMappingEntries], 
		int * arraySize, 
		mrdIfcGrpSPIResult_t * ret);
/* _MRDIFCGRP_H END ***************************************************/
#endif
