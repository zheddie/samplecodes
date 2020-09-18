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
/*..................................................................
  :.................................................................*/
  

//********************************************************************
// Include Section
//********************************************************************
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <qusec.h>           // Error code structures for iSeries APIs.
#include <errno.h>
#include <quscrtus.h>        // Create user space
#include <quscusat.h>
#include <qusdltus.h>        // Delete user space
#include <qusptrus.h>        // Get ptr to user space
#include <quscusat.h>        // Change user space attributes
#include <qusgen.h>

#include <mrdifcgrp.h>

//********************************************************************
// Local Functions
//********************************************************************
// get the user space pointer, if the user space doesn't exist, create one
// return: 0 - success, -1 - an error occurred
int getUserSpacePtr(char spaceName[20], void * spacePtr)
{
	char attr[10];
        int size = 16000000;
        char initValue = 0x00;
        char auth[10];
        char description[50];
        char replace[10];
	char exceptionMsgID[8];
        Qus_EC_t error;
	// incase the user space not found, we will try to create one here
        // initialize the variables
        memset(attr, ' ', 10);
        memcpy(auth, "*ALL      ", 10);
        memset(description, ' ', sizeof(description));
        memcpy(description, "Engine Config network information user space", sizeof(description));
        memcpy(replace, "*NO       ", 10); // make the replace as *NO, so we don't replace the existed one.

        error.Bytes_Available = 0;
        error.Bytes_Provided  = sizeof(Qus_EC_t);
        QUSCRTUS(spaceName, attr, size, &initValue,
                auth, description, replace, &error);

        if (error.Bytes_Available) {
            // the call failed
            memset(exceptionMsgID, 0x00, sizeof(exceptionMsgID));
            memcpy(exceptionMsgID, error.Exception_Id, sizeof(error.Exception_Id));
	    if(0 == strncmp(exceptionMsgID, "CPF9870", 7))
	    {
		 printf("QUSCRTUS return the error message, errorID is %s which means the User Space already existed, ignore this error.\n", exceptionMsgID);
	    }
	    else
	    {
            	printf("Call QUSCRTUS failed, errorID is %s \n", exceptionMsgID);
		return MRDIFCGRP_ERROR;
	    }
	    //return;
        }

        // Make this space an auto-extend space.
        struct {
            int              totalVarRecs;
            Qus_Vlen_Rec_3_t varRec3;
            char             autoExtend;
        } spaceAttr;

        spaceAttr.totalVarRecs = 1;
        spaceAttr.varRec3.Key = 3;
        spaceAttr.varRec3.Length_Vlen_Record = 1;
        spaceAttr.autoExtend = '1';

        char foundLib[10];
        error.Bytes_Available = 0;
        error.Bytes_Provided  = sizeof(Qus_EC_t);
        QUSCUSAT(foundLib, spaceName, &spaceAttr, &error);

        if (error.Bytes_Available) {
            memset(exceptionMsgID, 0x00, sizeof(exceptionMsgID));
            memcpy(exceptionMsgID, error.Exception_Id, sizeof(error.Exception_Id));
            printf("Call QUSCUSAT failed, errorID is %s \n", exceptionMsgID);
	    return MRDIFCGRP_ERROR;
        }
        // Get the pointer to the head of this user space.
        error.Bytes_Available = 0;
        error.Bytes_Provided  = sizeof(Qus_EC_t);
        QUSPTRUS(spaceName, spacePtr, &error);

        if (error.Bytes_Available) {
            memset(exceptionMsgID, 0x00, sizeof(exceptionMsgID));
            memcpy(exceptionMsgID, error.Exception_Id, sizeof(error.Exception_Id));
	    printf("Call QUSPTRUS failed, errorID is %s \n", exceptionMsgID);
	    return MRDIFCGRP_ERROR;
        }
	
        
	return MRDIFCGRP_OK;
}


void initUserSpacePtr(void * spacePtr)
{
	mrdIfgMapspaceHeader_t header;
	
	// retrieve the header info from the user space, if initialized, just return
	memcpy(&header, spacePtr, sizeof(mrdIfgNameMappingInfo_t));
	if(header.initialized)
		return;

	// initialize the header for the first time
	header.initialized = 1;
	header.Offset_List_Data = sizeof(mrdIfgMapspaceHeader_t);
	header.Number_List_Entries = 0;
	header.Size_Each_Entry = sizeof(mrdIfgNameMappingInfo_t);

	//Write the header info back to user space
	memcpy(spacePtr, &header, sizeof(mrdIfgMapspaceHeader_t));
}

// Write one Entry to the user space
// replace 0, can not add if the component existed
//         1, update the component mappping information
// This function will keep the component as an unique key
// return: 0 - success, -1 - error occurred
int write1EntryToUserSpace(void * spacePtr, mrdIfgNameMappingInfo_t * info, int replace)
{
	int offset;
	int numOfEntries;
	int EachEntrySize;
	int i;
	mrdIfgNameMappingInfo_t infoLoc;
	mrdIfcGrpComponentName componentName;
	mrdIfgMapspaceHeader_t header;
	
	memcpy(&infoLoc, info, sizeof(mrdIfgNameMappingInfo_t));
	memcpy(&header, spacePtr, sizeof(mrdIfgMapspaceHeader_t));
	strcpy(componentName, info->componentName);
	
	numOfEntries = header.Number_List_Entries;
	EachEntrySize = header.Size_Each_Entry;
	
	for(i = 0; i < numOfEntries; i++)
	{
		offset = header.Offset_List_Data + i * EachEntrySize;
		memcpy(&infoLoc, (char *)spacePtr + offset, sizeof(mrdIfgNameMappingInfo_t) );
		if(strcmp(componentName, infoLoc.componentName) == 0)
		{
			if(replace)
			{
				printf("Component already existed, component is %s, but the replace enabled, will replace the mapping information.\n", componentName);
				memcpy((char *)spacePtr + offset, info, sizeof(mrdIfgNameMappingInfo_t));
				return MRDIFCGRP_OK;
			}
			else
			{
				printf("Component already existed, component is %s\n",componentName);
				return MRDIFCGRP_DupComponentName;
			}
		}
	}
        // componentName not found, this will be a new component, add it to the end.
	printf("Component %s is new, will append the mapping information to the end.\n", componentName);
	
	offset = header.Offset_List_Data + i * EachEntrySize;
	memcpy((char *)spacePtr + offset, info, sizeof(mrdIfgNameMappingInfo_t));
	++header.Number_List_Entries;
	memcpy(spacePtr, &header, sizeof(mrdIfgMapspaceHeader_t));

	return MRDIFCGRP_OK;
}

// Read one Entry from the user space
// This function will keep the component as an unique key
// return: 0 - success, -1 - error occurred
int read1EntryFromUserSpace(void * spacePtr, mrdIfcGrpComponentName componentName, mrdIfgNameMappingInfo_t * info) // Return the Entry which match name
{
	int offset;
	int numOfEntries;
	int EachEntrySize;
	int i;
	//mrdIfcGrpComponentName componentName;
	mrdIfgMapspaceHeader_t header;
	mrdIfgNameMappingInfo_t infoLoc;
	
	memcpy(&header, spacePtr, sizeof(mrdIfgMapspaceHeader_t));
	
	//strcpy(componentName, info->componentName);

	numOfEntries = header.Number_List_Entries;
	EachEntrySize = header.Size_Each_Entry;
	for(i = 0; i < numOfEntries; i++)
	{
		offset = header.Offset_List_Data + i * EachEntrySize;
		memcpy(&infoLoc, (char *)spacePtr + offset, sizeof(mrdIfgNameMappingInfo_t) );
		if(strcmp(componentName, infoLoc.componentName) == 0)
		{
			memcpy(info, &infoLoc, sizeof(mrdIfgNameMappingInfo_t));
			return 0;
		}
	}
	
	printf("Component %s doesn't existed\n", componentName);
	return MRDIFCGRP_ComponentNameNotFound;
}

// Based on the unique component name in the user sapce, which should guarantee by write1EntryToUserSpace
int remove1EntryToUserSpace(void * spacePtr, mrdIfcGrpComponentName componentName) // Delete the Entry which matches the componentName
{
	int offset;
	int offsetDst;
	int offsetSrc;
	int numOfEntries;
	int EachEntrySize;
	int i;
	int pos = 0;
	int length;
	//mrdIfcGrpComponentName componentName;
	mrdIfgMapspaceHeader_t header;
	mrdIfgNameMappingInfo_t info;
	
	memcpy(&header, spacePtr, sizeof(mrdIfgMapspaceHeader_t));
	
	//strcpy(componentName, info.componentName);

	numOfEntries = header.Number_List_Entries;
	EachEntrySize = header.Size_Each_Entry;
	
	for(i = 0; i < numOfEntries; i++)
	{
		offset = header.Offset_List_Data + i * EachEntrySize;
		memcpy(&info, (char *)spacePtr + offset, sizeof(mrdIfgNameMappingInfo_t) );
		if(strcmp(componentName, info.componentName) == 0)
		{
			pos = i;
			break;
		}
	}

	if(i == numOfEntries)
	{
		printf("Component %s doesn't existed, so don't need to remove\n", componentName);
		return MRDIFCGRP_OK;
	}

	if(pos == numOfEntries - 1)// if the last one
	{
		offset = header.Offset_List_Data + (numOfEntries - 1) * EachEntrySize;
		memset((char *)spacePtr + offset, 0x0, sizeof(mrdIfgNameMappingInfo_t));
	}
	else
	{
		offsetDst = header.Offset_List_Data + pos * EachEntrySize;
		offsetSrc = header.Offset_List_Data + (pos + 1) * EachEntrySize;
		length = numOfEntries - pos;
		memcpy((char *)spacePtr + offsetDst, (char *)spacePtr + offsetSrc, length *  EachEntrySize);
		
		offset = header.Offset_List_Data + (numOfEntries - 1) * EachEntrySize;
		memset((char *)spacePtr + offset, 0x0, sizeof(mrdIfgNameMappingInfo_t));
	}

	--header.Number_List_Entries;
	memcpy(spacePtr, &header, sizeof(mrdIfgMapspaceHeader_t));

	return 0;

}

void readAllEntriesFromUserSpace(void * spacePtr, mrdIfgNameMappingInfo_t info[MRDIFCGRP_MaxMappingEntries], int * numOfEntries)
{
	int offset;
	int EachEntrySize;
	int i;
	mrdIfgMapspaceHeader_t header;
	
	memcpy(&header, spacePtr, sizeof(mrdIfgMapspaceHeader_t));
	
	*numOfEntries = header.Number_List_Entries;

	EachEntrySize = header.Size_Each_Entry;
	
	for(i = 0; i < *numOfEntries; i++)
	{
		offset = header.Offset_List_Data + i * EachEntrySize;
		memcpy(&info[i], (char *)spacePtr + offset, sizeof(mrdIfgNameMappingInfo_t) );
	}
}

//********************************************************************
// SPI Functions
//********************************************************************

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
void QmrdAddIfgMap(mrdIfgNameMappingInfo_t * info, 
		mrdIfcGrpSPIResult_t * ret)
{
#pragma datamodel(P128)
	char * spacePtr = NULL;
#pragma datamodel(pop)
	
	mrdIfgMapspaceHeader_t header;
	int result = 0;

	if((result = getUserSpacePtr(IFGCFG_USER_SPACE_NAME, &spacePtr)))
	{
		printf("Error occured in QmrdAddIfgMap");
		ret->result = result;
		return;
	}
	// init the space header information, and write the new header info to the user space back  
	initUserSpacePtr(spacePtr);
	result = write1EntryToUserSpace(spacePtr, info, 0);
	if(result)
	{
		ret->result = result;
	}
}



/**************************************************************/
/* Method:  QmrdRmvIfgMap                                     */
/* Purpose:  Delete the name mapping of component and         */
/*           interface group name in to the user space.       */
/*           This SPI does nothing related to networking      */
/*           interface group configuration.                   */
/*           It only operates on the mapping information.     */
/* Parameters:  mrdIfcGrpComponentName  (Input)               */
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
void QmrdRmvIfgMap(mrdIfcGrpComponentName componentName, 
		mrdIfcGrpSPIResult_t * ret)
{
#pragma datamodel(P128)
	char * spacePtr = NULL;
#pragma datamodel(pop)
	
	mrdIfgMapspaceHeader_t header;
	int result = 0;

	if((result = getUserSpacePtr(IFGCFG_USER_SPACE_NAME, &spacePtr)))
	{
		printf("Error occured in QmrdRmvIfgMap");
		ret->result = result;
		return;
	}
	
	// init the space header information, and write the new header info to the user space back  
	initUserSpacePtr(spacePtr);
	result = remove1EntryToUserSpace(spacePtr, componentName);
	if(result)
	{
		ret->result = result;
	}
}

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
void QmrdRtvIfgMap(mrdIfcGrpComponentName componentName, 
		mrdIfgNameMappingInfo_t * info, 
		mrdIfcGrpSPIResult_t * ret)
{
#pragma datamodel(P128)
	char * spacePtr = NULL;
#pragma datamodel(pop)
	
	mrdIfgMapspaceHeader_t header;
	int result = 0;

	if((result = getUserSpacePtr(IFGCFG_USER_SPACE_NAME, &spacePtr)))
	{
		printf("Error occured in QmrdRtvIfgMap");
		ret->result = result;
		return;
	}
	
	// init the space header information, and write the new header info to the user space back  
	initUserSpacePtr(spacePtr);
	result = read1EntryFromUserSpace(spacePtr, componentName, info);
	if(result)
	{
		ret->result = result;
	}
}

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
void QmrdAddIfcGrp(mrdIfcGrp_t * info, 
		mrdIfcGrpSPIResult_t * ret)
{
}

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
void QmrdRmvIfcGrp(mrdIfcGrpComponentName componentName, 
		mrdIfcGrpSPIResult_t * ret)
{
}

//********************************************************************
// SPI Functions, but only for internal usage
//********************************************************************

/**************************************************************/
/* Method:  QmrdAddIfgMapExt                                  */
/* Purpose:  Adds the name mapping of component and           */
/*           interface group name in to the user space.       */
/*           This SPI does nothing related to networking      */
/*           interface group configuration.                   */
/*           It only operates on the mapping information.     */
/* Parameters:  Pointer to mrdIfgNameMappingInfo_t  (Input)   */
/*              replace indicated if the mapping can replace  */
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
void QmrdAddIfgMapExt(mrdIfgNameMappingInfo_t * info, 
		int replace,
		mrdIfcGrpSPIResult_t * ret)
{
#pragma datamodel(P128)
	char * spacePtr = NULL;
#pragma datamodel(pop)
	
	mrdIfgMapspaceHeader_t header;
	int result = 0;

	if((result = getUserSpacePtr(IFGCFG_USER_SPACE_NAME, &spacePtr)))
	{
		printf("Error occured in QmrdAddIfgMapExt");
		ret->result = result;
		return;
	}

	// init the space header information, and write the new header info to the user space back  
	initUserSpacePtr(spacePtr);
	result = write1EntryToUserSpace(spacePtr, info, replace);
	if(result)
	{
		ret->result = result;
	}
}

/**************************************************************/
/* Method:  QmrdRtvAllIfgMaps                                 */
/* Purpose:  retrieves the mapping between component name and */
/*           interface group name from the user space         */
/*           into mrdIfgNameMappingInfo_t.                    */
/* Parameters:  infoArray   (Onput)                           */
/*              Pointer to arraySize  (Onput)                 */
/*              Pointer to mrdIfcGrpSPIResult_t (Input/Output)*/
/* Return value: None                                         */
/* Pre-conditions:  None                                      */
/* Post-conditions: If successful, all the interface group    */
/*                  and the component name mapping information*/
/*                  will be retrieved from the QUSRSYS network*/
/*                  information user space object and write to*/
/*                  infoArray                                 */
/*                  If an error occurred, the structure       */
/*                  mrdIfcGrpSPIResult_t will contain details */
/*                  about the error.                          */
/**************************************************************/
void QmrdRtvAllIfgMaps(mrdIfgNameMappingInfo_t infoArray[MRDIFCGRP_MaxMappingEntries], 
		int * arraySize, 
		mrdIfcGrpSPIResult_t * ret)
{
#pragma datamodel(P128)
	char * spacePtr = NULL;
#pragma datamodel(pop)
	
	mrdIfgMapspaceHeader_t header;
	int result = 0;

	if((result = getUserSpacePtr(IFGCFG_USER_SPACE_NAME, &spacePtr)))
	{
		printf("Error occured in QmrdRtvAllIfgMaps");
		ret->result = result;
		return;
	}
	
	// init the space header information, and write the new header info to the user space back  
	initUserSpacePtr(spacePtr);
	readAllEntriesFromUserSpace(spacePtr, infoArray, arraySize);
}
