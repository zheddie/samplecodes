/*..................................................................
  :.................................................................*/
  

//********************************************************************
// Include Section
//********************************************************************
#include <stdio.h>
#include <string.h>
#include <qmhsndpm.h>          /* Send program message API           */
#include <except.h>            /* Exception related declares         */
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <ctype.h>
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
// Function MAIN
//********************************************************************

int main(int argc, char *argv[]) 
{
	#pragma datamodel(P128)
    	char * spacePtr = NULL;
        #pragma datamodel(pop)
	
	
	mrdIfgMapspaceHeader_t header;

	//if(getUserSpacePtr(IFGCFG_USER_SPACE_NAME, &spacePtr))
	//	return -1;
	// init the space header information, and write the new header info to the user space back  
	//initUserSpacePtr(spacePtr);

	mrdIfgNameMappingInfo_t info;
	mrdIfgNameMappingInfo_t infoRet;
        int numOfEntries;
	mrdIfcGrpSPIResult_t result;
	mrdIfgNameMappingInfo_t infoArray[MRDIFCGRP_MaxMappingEntries];
	
	printf("***************************************************************\n");
	printf("*********************** Start Test Environment ****************\n");
	printf("***************************************************************\n");
	
	memset(&info, 0, sizeof(mrdIfgNameMappingInfo_t));
	memset(&result, 0, sizeof(mrdIfcGrpSPIResult_t));
	
	printf("/--- Add db2:ifcgrp1:ifc group for DB2----/\n");
	strcpy(info.componentName, "db2");
	strcpy(info.groupName, "ifcgrp1");
	strcpy(info.groupNameInfoDesc, "ifc group for DB2");
	QmrdAddIfgMap(&info, &result);
	printf("/--- result code is %d ----/\n", result.result);

	printf("/--- Add ifs:ifcgrp2:ifc group for IFS----/\n");
	strcpy(info.componentName, "ifs");
	strcpy(info.groupName, "ifcgrp2");
	strcpy(info.groupNameInfoDesc, "ifc group for IFS");
	QmrdAddIfgMap(&info, &result);
	printf("/--- result code is %d ----/\n", result.result);

	printf("/--- Add sysobj:ifcgrp3:ifc group for system objects----/\n");
	strcpy(info.componentName, "sysobj");
	strcpy(info.groupName, "ifcgrp3");
	strcpy(info.groupNameInfoDesc, "ifc group for system objects");
	QmrdAddIfgMap(&info, &result);
	printf("/--- result code is %d ----/\n", result.result);
	
	printf("/--- Add userdefcomp1:ifcgrp11:ifc group for user defiend 1----/\n");
	strcpy(info.componentName, "userdefcomp1");
	strcpy(info.groupName, "ifcgrp11");
	strcpy(info.groupNameInfoDesc, "ifc group for user defiend 1");
	QmrdAddIfgMap(&info, &result);
	printf("/--- result code is %d ----/\n", result.result);

	printf("/--- Add db2:ifcgrp1dup:ifc group for user DB2 dup----, should fail to add ---/\n");
	strcpy(info.componentName, "db2");
	strcpy(info.groupName, "ifcgrp1dup");
	strcpy(info.groupNameInfoDesc, "ifc for DB2 dup");
	QmrdAddIfgMap(&info, &result);
	printf("/--- result code is %d ----/\n", result.result);
	
	printf("/--- Add ifs:ifcgroup3dup:ifc group for IFS dup----, should fail to add ---/\n");
	strcpy(info.componentName, "ifs");
	strcpy(info.groupName, "ifcgroup3dup");
	strcpy(info.groupNameInfoDesc, "ifc group for IFS dup");
	QmrdAddIfgMap(&info, &result);
	printf("/--- result code is %d ----/\n", result.result);
	
	printf("/--- Add sysobj:sysobj:ifc group for IFS dup----, should fail to add ---/\n");
	strcpy(info.componentName, "sysobj");
	strcpy(info.groupName, "ifcgrpobj");
	strcpy(info.groupNameInfoDesc, "ifc group for system objects dup");
	QmrdAddIfgMap(&info, &result);
	printf("/--- result code is %d ----/\n", result.result);
		
	printf("/--- Add userdefcomp1:userdefcomp1:ifc group for user defiend 1 dup----, should fail to add ---/\n");
	strcpy(info.componentName, "userdefcomp1");
	strcpy(info.groupName, "userdefcomp1");
	strcpy(info.groupNameInfoDesc, "ifc group for user defiend 1 dup");
	QmrdAddIfgMap(&info, &result);
	printf("/--- result code is %d ----/\n", result.result);
	
	printf("/--- Add userdefcomp2:userdefcomp2:ifc group for user defiend 2----/\n");
	strcpy(info.componentName, "userdefcomp2");
	strcpy(info.groupName, "userdefcomp2");
	strcpy(info.groupNameInfoDesc, "ifc group for user defiend 2");
	QmrdAddIfgMap(&info, &result);
	printf("/--- result code is %d ----/\n", result.result);

	/* Retrieve the mappping infomation */
	printf("/--- Retrieve the db2 map----/\n");
	memset(&infoRet, 0, sizeof(mrdIfgNameMappingInfo_t));
	QmrdRtvIfgMap("db2", &infoRet, &result);
	printf("/--- result code is %d ----/\n", result.result);

	printf("/********** Start dumpping group information **********************/\n");
	printf("componentName is %s\n", infoRet.componentName);
	printf("groupName is %s\n", infoRet.groupName);
	printf("groupNameInfoDesc is %s\n", infoRet.groupNameInfoDesc);
	printf("/********** End dumpping group information ************************/\n");

	memset(&infoRet, 0, sizeof(mrdIfgNameMappingInfo_t));
	printf("/--- Retrieve the ifc map----/\n");
	QmrdRtvIfgMap("ifs", &infoRet, &result);
	printf("/--- result code is %d ----/\n", result.result);

	printf("/********** Start dumpping group information **********************/\n");
	printf("componentName is %s\n", infoRet.componentName);
	printf("groupName is %s\n", infoRet.groupName);
	printf("groupNameInfoDesc is %s\n", infoRet.groupNameInfoDesc);
	printf("/********** End dumpping group information ************************/\n");
	
	memset(&infoRet, 0, sizeof(mrdIfgNameMappingInfo_t));
	QmrdRtvIfgMap("sysobj", &infoRet, &result);
	printf("/--- result code is %d ----/\n", result.result);

	printf("/********** Start dumpping group information **********************/\n");
	printf("componentName is %s\n", infoRet.componentName);
	printf("groupName is %s\n", infoRet.groupName);
	printf("groupNameInfoDesc is %s\n", infoRet.groupNameInfoDesc);
	printf("/********** End dumpping group information ************************/\n");
	
	memset(&infoRet, 0, sizeof(mrdIfgNameMappingInfo_t));
	QmrdRtvIfgMap("userdefcomp1", &infoRet, &result);
	printf("/--- result code is %d ----/\n", result.result);

	printf("/********** Start dumpping group information **********************/\n");
	printf("componentName is %s\n", infoRet.componentName);
	printf("groupName is %s\n", infoRet.groupName);
	printf("groupNameInfoDesc is %s\n", infoRet.groupNameInfoDesc);
	printf("/********** End dumpping group information ************************/\n");

	memset(&infoRet, 0, sizeof(mrdIfgNameMappingInfo_t));
	QmrdRtvIfgMap("userdefcomp2", &infoRet, &result);
	printf("/--- result code is %d ----/\n", result.result);

	printf("/********** Start dumpping group information **********************/\n");
	printf("componentName is %s\n", infoRet.componentName);
	printf("groupName is %s\n", infoRet.groupName);
	printf("groupNameInfoDesc is %s\n", infoRet.groupNameInfoDesc);
	printf("/********** End dumpping group information ************************/\n");
	
	memset(infoArray,0, MRDIFCGRP_MaxMappingEntries * sizeof(mrdIfgNameMappingInfo_t));
	QmrdRtvAllIfgMaps(infoArray, &numOfEntries, &result);
	printf("/********** Start dumpping all group information **********************/\n");
	printf("Entry number is %d\n", numOfEntries);
	for(int i = 0; i < MRDIFCGRP_MaxMappingEntries; i++)
	{
		printf("Entry %d, %s:%s:%s\n", i, infoArray[i].componentName,  infoArray[i].groupName, infoArray[i].groupNameInfoDesc);
	}
	printf("/********** End dumpping all group information **********************/\n");
	
	
	printf("/--- Remove sysobj Component ----/\n");
	QmrdRmvIfgMap("sysobj", &result);
	printf("/--- result code is %d ----/\n", result.result);

	memset(&infoRet, 0, sizeof(mrdIfgNameMappingInfo_t));
	QmrdRtvIfgMap("sysobj", &infoRet, &result);
	printf("/--- result code is %d ----/\n", result.result);

	printf("/********** Start dumpping group information **********************/\n");
	printf("componentName is %s\n", infoRet.componentName);
	printf("groupName is %s\n", infoRet.groupName);
	printf("groupNameInfoDesc is %s\n", infoRet.groupNameInfoDesc);
	printf("/********** End dumpping group information ************************/\n");

	memset(infoArray,0, MRDIFCGRP_MaxMappingEntries * sizeof(mrdIfgNameMappingInfo_t));
	QmrdRtvAllIfgMaps(infoArray, &numOfEntries, &result);
	printf("/********** Start dumpping all group information **********************/\n");
	printf("Entry number is %d\n", numOfEntries);
	for(int i = 0; i < MRDIFCGRP_MaxMappingEntries; i++)
	{
		printf("Entry %d, %s:%s:%s\n", i, infoArray[i].componentName,  infoArray[i].groupName, infoArray[i].groupNameInfoDesc);
	}
	printf("/********** End dumpping all group information **********************/\n");
	

	printf("/--- Add sysobj:ifcgrp33:ifc group for system objects dup ----/\n");
	strcpy(info.componentName, "sysobj");
	strcpy(info.groupName, "ifcgrp33");
	strcpy(info.groupNameInfoDesc, "ifc group for system objects dup");
	QmrdAddIfgMap(&info, &result);
	printf("/--- result code is %d ----/\n", result.result);
	
	memset(&infoRet, 0, sizeof(mrdIfgNameMappingInfo_t));
	QmrdRtvIfgMap("sysobj", &infoRet, &result);
	printf("/--- result code is %d ----/\n", result.result);

	printf("/********** Start dumpping group information **********************/\n");
	printf("componentName is %s\n", infoRet.componentName);
	printf("groupName is %s\n", infoRet.groupName);
	printf("groupNameInfoDesc is %s\n", infoRet.groupNameInfoDesc);
	printf("/********** End dumpping group information ************************/\n");
	
	printf("/--- Remove ifs Component ----/\n");
	QmrdRmvIfgMap("ifs", &result);
	printf("/--- result code is %d ----/\n", result.result);

	memset(infoArray,0, MRDIFCGRP_MaxMappingEntries * sizeof(mrdIfgNameMappingInfo_t));
	QmrdRtvAllIfgMaps(infoArray, &numOfEntries, &result);
	printf("/********** Start dumpping all group information **********************/\n");
	printf("Entry number is %d\n", numOfEntries);
	for(int i = 0; i < MRDIFCGRP_MaxMappingEntries; i++)
	{
		printf("Entry %d, %s:%s:%s\n", i, infoArray[i].componentName,  infoArray[i].groupName, infoArray[i].groupNameInfoDesc);
	}
	printf("/********** End dumpping all group information **********************/\n");

	printf("/--- Remove db2 Component ----/\n");
	QmrdRmvIfgMap("db2", &result);
	printf("/--- result code is %d ----/\n", result.result);

	memset(infoArray,0, MRDIFCGRP_MaxMappingEntries * sizeof(mrdIfgNameMappingInfo_t));
	QmrdRtvAllIfgMaps(infoArray, &numOfEntries, &result);
	printf("/********** Start dumpping all group information **********************/\n");
	printf("Entry number is %d\n", numOfEntries);
	for(int i = 0; i < MRDIFCGRP_MaxMappingEntries; i++)
	{
		printf("Entry %d, %s:%s:%s\n", i, infoArray[i].componentName,  infoArray[i].groupName, infoArray[i].groupNameInfoDesc);
	}
	printf("/********** End dumpping all group information **********************/\n");

	printf("/--- Remove sysobj Component ----/\n");
	QmrdRmvIfgMap("sysobj", &result);
	printf("/--- result code is %d ----/\n", result.result);

	memset(infoArray,0, MRDIFCGRP_MaxMappingEntries * sizeof(mrdIfgNameMappingInfo_t));
	QmrdRtvAllIfgMaps(infoArray, &numOfEntries, &result);
	printf("/********** Start dumpping all group information **********************/\n");
	printf("Entry number is %d\n", numOfEntries);
	for(int i = 0; i < MRDIFCGRP_MaxMappingEntries; i++)
	{
		printf("Entry %d, %s:%s:%s\n", i, infoArray[i].componentName,  infoArray[i].groupName, infoArray[i].groupNameInfoDesc);
	}
	printf("/********** End dumpping all group information **********************/\n");

	
	printf("/--- Remove nullComp Component, should fail ----/\n");
	QmrdRmvIfgMap("nullComp", &result);
	printf("/--- result code is %d ----/\n", result.result);

	memset(infoArray,0, MRDIFCGRP_MaxMappingEntries * sizeof(mrdIfgNameMappingInfo_t));
	QmrdRtvAllIfgMaps(infoArray, &numOfEntries, &result);
	printf("/********** Start dumpping all group information **********************/\n");
	printf("Entry number is %d\n", numOfEntries);
	for(int i = 0; i < MRDIFCGRP_MaxMappingEntries; i++)
	{
		printf("Entry %d, %s:%s:%s\n", i, infoArray[i].componentName,  infoArray[i].groupName, infoArray[i].groupNameInfoDesc);
	}
	printf("/********** End dumpping all group information **********************/\n");
	
	printf("/--- Remove userdefcomp1 Component ----/\n");
	QmrdRmvIfgMap("userdefcomp1", &result);
	printf("/--- result code is %d ----/\n", result.result);

	memset(infoArray,0, MRDIFCGRP_MaxMappingEntries * sizeof(mrdIfgNameMappingInfo_t));
	QmrdRtvAllIfgMaps(infoArray, &numOfEntries, &result);
	printf("/********** Start dumpping all group information **********************/\n");
	printf("Entry number is %d\n", numOfEntries);
	for(int i = 0; i < MRDIFCGRP_MaxMappingEntries; i++)
	{
		printf("Entry %d, %s:%s:%s\n", i, infoArray[i].componentName,  infoArray[i].groupName, infoArray[i].groupNameInfoDesc);
	}
	printf("/********** End dumpping all group information **********************/\n");

	printf("/--- Remove userdefcomp2 Component ----/\n");
	QmrdRmvIfgMap("userdefcomp2", &result);
	printf("/--- result code is %d ----/\n", result.result);

	memset(infoArray,0, MRDIFCGRP_MaxMappingEntries * sizeof(mrdIfgNameMappingInfo_t));
	QmrdRtvAllIfgMaps(infoArray, &numOfEntries, &result);
	printf("/********** Start dumpping all group information **********************/\n");
	printf("Entry number is %d\n", numOfEntries);
	for(int i = 0; i < MRDIFCGRP_MaxMappingEntries; i++)
	{
		printf("Entry %d, %s:%s:%s\n", i, infoArray[i].componentName,  infoArray[i].groupName, infoArray[i].groupNameInfoDesc);
	}
	printf("/********** End dumpping all group information **********************/\n");

	
	
	
	for(int i = 0; i < MRDIFCGRP_MaxMappingEntries; i++)
	{
		char group[20];
		char comp[20];
		char desc[50];
		
		sprintf(group, "ifcgrp%d", i);
		sprintf(comp, "comp%d", i);
		sprintf(desc, "component group description %d", i);
		memset(&info, 0, sizeof(mrdIfgNameMappingInfo_t));
		
		strcpy(info.componentName, comp);
		strcpy(info.groupName, group);
		strcpy(info.groupNameInfoDesc, desc);

		QmrdAddIfgMap(&info, &result);
	}

	memset(infoArray,0, MRDIFCGRP_MaxMappingEntries * sizeof(mrdIfgNameMappingInfo_t));
	QmrdRtvAllIfgMaps(infoArray, &numOfEntries, &result);
	
	for(int i = 0; i < MRDIFCGRP_MaxMappingEntries; i++)
	{
		printf("Entry %d, %s:%s:%s\n", i, infoArray[i].componentName,  infoArray[i].groupName, infoArray[i].groupNameInfoDesc);
	}
	printf("/********** End dumpping all group information **********************/\n");

	printf("/--- Remove comp25 Component ----/\n");
	QmrdRmvIfgMap("comp25", &result);
	printf("/--- result code is %d ----/\n", result.result);
	printf("/--- Remove comp15 Component ----/\n");
	QmrdRmvIfgMap("comp15", &result);
	printf("/--- result code is %d ----/\n", result.result);
	printf("/--- Remove comp5 Component ----/\n");
	QmrdRmvIfgMap("comp5", &result);
	printf("/--- result code is %d ----/\n", result.result);
	printf("/--- Remove comp0 Component ----/\n");
	QmrdRmvIfgMap("comp0", &result);
	printf("/--- result code is %d ----/\n", result.result);
	printf("/--- Remove comp31 Component ----/\n");
	QmrdRmvIfgMap("comp31", &result);
	printf("/--- result code is %d ----/\n", result.result);
	printf("/--- Remove comp33 Component ----/\n");
	QmrdRmvIfgMap("comp33", &result);
	printf("/--- result code is %d ----/\n", result.result);
	
	memset(infoArray,0, MRDIFCGRP_MaxMappingEntries * sizeof(mrdIfgNameMappingInfo_t));
	QmrdRtvAllIfgMaps(infoArray, &numOfEntries, &result);
	printf("/********** Start dumpping all group information **********************/\n");
	printf("Entry number is %d\n", numOfEntries);
	for(int i = 0; i < MRDIFCGRP_MaxMappingEntries; i++)
	{
		printf("Entry %d, %s:%s:%s\n", i, infoArray[i].componentName,  infoArray[i].groupName, infoArray[i].groupNameInfoDesc);
	}
	printf("/********** End dumpping all group information **********************/\n");




	printf("***************************************************************\n");
	printf("*********************** End Test Environment ******************\n");
	printf("***************************************************************\n");
	return 0;
}
