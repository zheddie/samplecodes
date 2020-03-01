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
/*     5769-SS1                                                     */
/*     5722-SS1                                                     */
/*     5770-SS1                                                     */
/*                                                                  */
/* (C)Copyright IBM Corp.  1997, 2013                               */
/*                                                                  */
/* The Source code for this program is not published  or otherwise  */
/* divested of its trade secrets,  irrespective of what has been    */
/* deposited with the U.S. Copyright Office.                        */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
//      
//---------------------------------------------------------------------------
//   
//
//  [Words about the classes, structures, etc. coded in this file.]
//
//  Bounds to object   :  *                 [SRVPGM or PGM]
//  Program domain     :  *                 [User or System]
//  Program state      :  *                 [User or System]
//  Activation Group   :  *                 [*Caller or name or special note]
//  Owner              :  *                 [owning profile name]
//  Adopt owners Auth  :  *                 [yes or no]
//  Runs under Usr Prf :  *                 [yes or no or special notes]
//  Public Authority   :  *                 [*USE, *EXCLUDE, etc.]
//
//---------------------------------------------------------------------------
//
// Change Activity:
//
// flg reason    vers date   userid    comment
// --- --------- ---- ------ -------  ---------------------------------
// $A0 P3669346  4d20 981415 wlynch : New file
// $A1 P9914131  5d10 000202 wlynch : Added change object date/time value
// $A2           5d10        wlynch : Changes made and PTR forgotten     
// $A3 P9933983  5d10 000928 wlynch : Additional locking for storage integrity
//                                    & weak consistency.
// $A4 P9937753  5d10 001114 wlynch : Made change to page alignment calc.
//                                  : Make fix to resolveObject/doesExist code
// $B0 p9942638  5p10 010207 wlynch : Elminated redundant calls to the open
//                                    Secondary Repository method
// $C0 D98669.2  v5r2 010531 wlynch : Support for expanded repository area
//                                    Global change where m_objectHdr was
//                                    replace with m_colObjAttr
//                                  : Store away mi tod timstamp
//                                  : Added rp data source support
// $B1 D98668.1  v5r2 010815 wlynch : Misc changes 
// $B2 P9951708  v5r2 010912 wlynch : LockSpace changes, object attr changes
// $B3 P9955292  5D20 011020 wlynch : Add EPTCALL around apis 
// $B4 P9954091  5D20 011101 wlynch : Fsroute change for pex trace
// $B5 P9957301  5D20 011119 wlynch : Crtdupobj changes
// $B6 P9957301  5D20 011119 wlynch : Qtemp resolution change.
// $B7 P9958445  5D20 011205 wlynch : Add missed EPTCALLs
// $B8 P9A03438  5f20 020305 wlynch : fixCollectionEndAttributes change
// $B9 P9A06471  5f20 020418 wlynch : Going beyond rp array problem     
// $BA P9A19565  5p20 021029 wlynch : Add additional locking when a repository
//                                    is open for write such that save ops
//                                    will not save the object.
// $BB PSE08587  5p20 021203 wlynch : Fixed problem where repositories are
//                                    added by collection services at
//                                    different times.
// $C1 P9A22184  5D30 021213 wlynch : Independent ASP fix for BSFs
// $C2 D99340.1  5D30 030207 wlynch : Machine chooses attributes change on create
// $C3 P9A29820  5D30 030505 wlynch : open for write unlock lock fix
// $C4 P9A36642  5p20 030805 wlynch : Read repostitory fix
// $C5           5D30 030811 wlynch : Added option to create mgtcol with use
//                                    authority.
// $D0 D99936    5D40 040630 nguyen1d Added a new field to handle long_time_t.  Also
//                                    change getters for time to return a long_time_t instead
//                                    of time_t.
// $D1 PFW186243 6P10 090910 forman : add CCA update to destroy & set exp time
// $D2 PFW190466 6P10 090923 sbsmith: Do not reset nextAvailAddr in open()
// $D3 PSE37942  6P10 090408 sbsmith : Update local copy of repository array 
//                                     before creating a new repository
// $D4 PFW205095 6P10 090408 sbsmith : Don't need exclusive read lock in 
//                                     openRepository for *PEX mgtcols
// $D5 PFW180593 7D20 100803 sbsmith : Add mgtcol type to collection attributes
// $D6 PFW573792 7D20 111006 sbsmith : Initialize OIR text
// $D7 PFW585264 6P10 120615 sbsmith : Reset nextAvailAddr in open() if NULL
// $D8 PFW589684 7D20 120822 sbsmith : Fix problem setting mgtcol type
// $D9 PFW585230 7P10 130617 sbsmith : Get exclusive lock on object before enabling
//                                     the truncation of any byte stream files
//
//---------------------------------------------------------------------------
#include "qypscopyriCopyrightV4r3m0V4r3m0.H"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>  
#include <milib.h>
#include <mispcobj.h>
#include <miptrnam.h> 
#include <setacst.h>
#include <ensobj.h>
#include <new.h>
#include <matsobj.cinc>
#include <matobjif.cinc>
#include "include/wwsymaut.H"
#include <mods.cleinc>
#include <matmatr.cleinc>
#include <qlicobjd.cleinc>

#include "qypscolobjMiCollectionObject.H"
#include "qypsmxlockLock.H"
#include "qypspkblidPacketableId.H"
#include "qypsexceptException.H"
#include "qypstrycatTryCatch.H"
#include "qypsprintfPrintf.H"	// @D3A
#include "qypsconvrtCCSIDConverter.H"
#include "wsyqsysm.H"
#include "qushdler.H"
#include "qypsapirtcApiRc.H"	
                                   //$A3A 
#include <locksl.cleinc>
#include <unlocksl.cleinc>
#include <mattod.cleinc>

#include <stsppo.cleinc>
                                   //@B3A EPTCALL macros
#include "licall.h"	
#include "mats.cleinc"
#include "stsppo.cleinc"
#include "qwcwcb.cinc"
#include <syncstg.cleinc>

//ypsMutex ypsMiCollectionObject::;

ypsCollectionAttributes::ypsCollectionAttributes( )
: m_repositoryCount  ( 0 )
, m_objRetentionHours( 0 )
, m_totalSizeBase    ( 0 )
, m_defaultInterval  ( 0 )
                                   // Increment with any internal structure
                                   // or object changes that would make it
                                   // incompatible with old code...
, m_levelIndicator   ( YpsCollObjectCurLevel )
, m_isActiveFlag     ( 0 )
, m_fixEndFlag       ( 0 )
, m_isArchived       ( 0 )
, m_toGraphData      ( 0 )
, m_mgtcolType       ( YpsMgtcolTypeUNKNOWN )         // @D5A
                                   //@C0A The object header was incorporated 
                                   //@C0A into the object attributes header
, m_collectionStart  ( 0 )
, m_collectionEnd    ( 0 )
, m_collectionExpire ( 0 )
, m_miTodStart	     ( 0 )
, m_miTodEnd         ( 0 )
, m_collectionStart_long ( 0 )
, m_collectionEnd_long ( 0 )
, m_nextAvailAddr( NULL )

{
    memset( m_startTimestamp, 0, sizeof( m_startTimestamp ) );
    memset( m_endTimestamp  , 0, sizeof( m_endTimestamp   ) );
    memset( m_logicalSSN    , 0, sizeof( m_logicalSSN     ) );
    memset( m_reserved1     , 0, sizeof(m_reserved1       ) );
                                   //@C0A The object header was incorporated 
                                   //@C0A into the object attributes header
    memset( m_char1200 , 0x00, sizeof(m_char1200)  );
    memset( m_char4401 , 0x00, sizeof(m_char4401)  );
    memset( m_reserved2, 0x00, sizeof(m_reserved2) );
    memset( m_reserved3, 0x00, sizeof(m_reserved3) );
    memset( m_rpArray  , 0x00, sizeof(m_rpArray)   );
    memset( m_reserved4, 0x00, sizeof(m_reserved4) );

}
ypsMiCollectionObject::ypsMiCollectionObject
( char* objLibrary, char* objName )
: m_objSysPtr   (      NULL  )
, m_objBaseSpacePtr(   NULL  )
, m_objLibSysPtr(      NULL  )
, m_userProfileSysPtr( NULL  )
, m_objLibrary  ( objLibrary )
, m_objName     ( objName    )
, m_sharedReadLock    ( _LSRD_LOCK, 60 )
, m_exclusiveLock     ( _LENR_LOCK, 0, YPS_PROCESS_SCOPE, YPS_IMMED_LOCK) //@D9C
                                   //@BAC Redefine existing lock
, m_exclusiveReadLock ( _LSUP_LOCK, 60 )
, m_rpAttrElem( )
, m_objMutex( )
, m_rpLocalIdx( 0 )
, m_compositeObjHdr( NULL )
, m_colObjAttr( NULL )
{
    m_objLibrary.stripTrailing( );
    m_objName.stripTrailing( );
    memset( m_rpLocalArray, 0x00, sizeof(m_rpLocalArray) );
}
ypsMiCollectionObject::ypsMiCollectionObject
( char* objPath )
: m_objSysPtr   (      NULL  )
, m_objBaseSpacePtr(   NULL  )
, m_objLibSysPtr(      NULL  )
, m_userProfileSysPtr( NULL  )
, m_objLibrary  ( objPath )
, m_objName     ( objPath )
, m_sharedReadLock( _LSRD_LOCK, 60 )
, m_exclusiveLock ( _LENR_LOCK, 0, YPS_PROCESS_SCOPE, YPS_IMMED_LOCK ) //@D9C
                                   //@BAC Redefine existing lock
, m_exclusiveReadLock ( _LSUP_LOCK, 60 )
, m_rpAttrElem( )
, m_objMutex( )
, m_rpLocalIdx( 0 )
, m_compositeObjHdr( NULL )
, m_colObjAttr( NULL )
{
    int strLen = strlen( objPath );
    for( int i = 0; i < strLen; i++ )
    {
	if( objPath[i] == '/' )
	{
	    m_objLibrary = m_objLibrary.extractSubstring(0, i);
	    m_objName    = m_objName.extractSubstring( (i+1)
						       , (strLen - i -1)  );
	    break;
	}
    }
    m_objLibrary.stripTrailing( );
    m_objName.stripTrailing( );
    memset( m_rpLocalArray, 0x00, sizeof(m_rpLocalArray) );

}

ypsMiCollectionObject::ypsMiCollectionObject
( _SYSPTR objPtr )
: m_objSysPtr   ( objPtr     )
, m_objBaseSpacePtr( (_SPCPTR) _SETSPPFP( objPtr )  )
, m_objLibSysPtr(      NULL  )
, m_userProfileSysPtr( NULL  )
, m_objLibrary  (  )
, m_objName     (  )
, m_sharedReadLock( _LSRD_LOCK, 60 ) 
, m_exclusiveLock ( _LENR_LOCK, 0, YPS_PROCESS_SCOPE, YPS_IMMED_LOCK ) //@D9C
                                   //@BAC Redefine existing lock
, m_exclusiveReadLock ( _LSUP_LOCK, 60 ) 
, m_rpAttrElem( )
, m_objMutex( )
, m_rpLocalIdx( 0 )
, m_compositeObjHdr( NULL )
, m_colObjAttr( NULL )
{
    resolveObject( 1 );

    memset( m_rpLocalArray, 0x00, sizeof(m_rpLocalArray) );

    char tempName[32];

    materializeSystemObject( );

    strncpy( tempName, m_matObjTpl.Context.Name, 10 );
    m_objLibrary = tempName;

    strncpy( tempName, m_matObjTpl.Object.Name, 10 );
    m_objName = tempName;
    open( );
}

ypsMiCollectionObject::~ypsMiCollectionObject()
{
    ypsError theError;

#pragma exception_handler (ypsHandleBasicException, theError,0,_C2_MH_ESCAPE )
    YPS_TRY(theError, "*", MSGACTION_REMOVE)
    {
                                   //@C0A If the pointer has been set continue
	if( m_colObjAttr )
	{
	    int rpCount = m_colObjAttr->m_repositoryCount;
	    for( int x = 0; x < rpCount; x++)
	    {
		if( m_rpLocalArray[x] )
		{
		    delete m_rpLocalArray[x];
		    m_rpLocalArray[x] = NULL;
		}
	    }
	}
    }
# pragma disable_handler
    YPS_CATCH(theError)
    {

    }

}
                                   //@C5 New create2 method is now called by
                                   //@C5 the old create method.
void ypsMiCollectionObject::create
( char*   objOwner
  , int   defaultInterval
  , char* objAttrStr 
  , char* textDesc
  , int extendedRpArea   
)
{
    create2( objOwner
	     , defaultInterval
	     , objAttrStr
	     , textDesc
	     , extendedRpArea
	     , YpsMgtcolExclude
	     );
}


                                   //@C5 New create2 method that allows a mgtcol
                                   //@C5 object to be created with *use authority
void ypsMiCollectionObject::create2
( char*   objOwner
  , int   defaultInterval
  , char* objAttrStr 
  , char* textDesc
  , int extendedRpArea   
  , int mgtcolAuthority
)
{
                                   //@C0A If the object exists send error
    if( doesExist( ) )
    {
	char rplText[50];
	sprintf( rplText, "%-10.10s%-10.10s%-10.10s"
		 , (char*)m_objName
		 , (char*)m_objLibrary
				   //$A4A This was in error, now the string is
				   //$A4A properly formed.
		 , "MGTCOL"
		 );
	sendError( "CPF2112", rplText, strlen(rplText) );

    }

    resolveLibrary( );

    createSpace( );

    m_objBaseSpacePtr = (_SPCPTR) _SETSPPFP( m_objSysPtr ) ;

    lockspace( m_objBaseSpacePtr );
                                   // Before the object is to be inserted into
                                   // the library, the standard composite hdr
                                   // must be initialized properly.
                                   //
				   // Assign and initialize pointer/space
   m_compositeObjHdr = (ypsStdCompositeObjHdr*)m_objBaseSpacePtr;
   ypsStdCompositeObjHdr tmpCompHdr;
   *m_compositeObjHdr = tmpCompHdr;

                                   // If the mgtcol object will not have the
                                   // extended repository area set the pointers
                                   // accordingly based of space for 255 
                                   // repository pointers.
    if( !extendedRpArea )
    {
                                   // Max Rp pointers is 255.
	m_compositeObjHdr->m_maxRpPointers = MaxRepositoryNumber;
                                   // Zerp out the rp pointer array. The size
                                   // of the array in the struct is 255
	memset( m_compositeObjHdr->m_rpPointerArray
		, 0x00	
		, sizeof( m_compositeObjHdr->m_rpPointerArray )
		);
                                   // Assign and initialize the mgtcol base
                                   // attributes pointer/space.
	char* tptr
	  = (char*)(m_objBaseSpacePtr) +  sizeof(ypsStdCompositeObjHdr);
	m_colObjAttr = (ypsCollectionAttributes*)tptr;


	ypsCollectionAttributes tmpHdr;
	*m_colObjAttr = tmpHdr;


    }
                                   // Else, setup the mgtcol object to handle
                                   // 65535 repository pointers.
    else
    {
                                   // Max Rp pointers is 65535
	m_compositeObjHdr->m_maxRpPointers = MaxRepositoryNumberExt;
                                   // Zero out the rp pointer array for 65535
                                   // pointers.
 	memset( m_compositeObjHdr->m_rpPointerArray
		, 0x00	
		, (16 * MaxRepositoryNumberExt )
		);
                                   // Assign and initialize the mgtcol base
                                   // attributes pointer/space after the
                                   // Standard composite object space which  
                                   // set aside space 65535 Rp pointers.
	char* tptr
	  = (char*)(m_objBaseSpacePtr) + CompositeObjHdrSizeExt ;
	m_colObjAttr = (ypsCollectionAttributes*)tptr;

	ypsCollectionAttributes tmpHdr;
	*m_colObjAttr = tmpHdr;


    }
    m_colObjAttr->m_nextAvailAddr = m_colObjAttr->m_userAreaPtr;

    m_compositeObjHdr->m_rpPointerArray
      [m_compositeObjHdr->m_numberOfPtrs] = m_objSysPtr;
    ++m_compositeObjHdr->m_numberOfPtrs;

    memset( m_colObjAttr->m_rpArray
	    , 0x00
	    , sizeof(m_colObjAttr->m_rpArray)
	    );

    unlockspace( m_objBaseSpacePtr );

                                   // Now prepare to insert the object into
                                   // the library.
    char objlibname[21];

    sprintf( objlibname, "%-10.10s%-10.10s"
	     , (char*)m_objName
	     , (char*)m_objLibrary
	     );

    ypsTextDesc textDescTpl;
    memset(&textDescTpl, 0, sizeof(ypsTextDesc)); // @D6A 

    if( textDesc )
    {
	if( strlen( textDesc ) < ( sizeof( textDescTpl ) - 1 ) )
	{
	    strcpy( textDescTpl.m_text, textDesc );
	}
	else
	{
	    strncpy( textDescTpl.m_text
		     , textDesc
		     , ( sizeof( textDescTpl ) - 1 )
		     ) ;
	}
	textDescTpl.m_length = strlen( textDescTpl.m_text );
    }

    ypsAttrStr attrStrTpl;

    int theStrLen = 0;
                                   //@B2A Changed to ensure all characters
                                   //@B2A are used for the objAttr

    int tplStrSize = sizeof( attrStrTpl.m_AttrStr );
    if( objAttrStr )
    {
	theStrLen = strlen( objAttrStr );
	if( theStrLen < tplStrSize )
	{
	    
	    strcpy( attrStrTpl.m_AttrStr, objAttrStr );
	}
	else
	{
	    theStrLen = tplStrSize;
	    strncpy( attrStrTpl.m_AttrStr
		     , objAttrStr
		     , theStrLen
		     ) ;
	}
	attrStrTpl.m_length = theStrLen;
    }

    struct objAuthority
    {
	uint16 numberOfEntries;
	char   authority[10];
    };

    objAuthority objA;

    objA.numberOfEntries = 1;
                                   //@C5 If called with USE authority give it
                                   //@C5 use authority.
    if( mgtcolAuthority == YpsMgtcolUse )
    {
	memcpy( objA.authority, "*USE      ", 10 );
    }
    else
    {
	memcpy( objA.authority, "*EXCLUDE  ", 10 );
    }
 
    INSERT_OBJECT( objlibname
		   ,"*MGTCOL"
		   , &objA
		   , m_objSysPtr
		   , m_objLibSysPtr
		   , &textDescTpl
		   , &attrStrTpl
		   );
    char objName[30];
    char newOwner[10];
    char revokeValue = 'R';
    int  result = 0;

    memset( newOwner,' ',sizeof(newOwner) );
    memcpy( newOwner, objOwner, strlen( objOwner ) );

    memset( objName,' ',30 );
    memcpy( objName, objlibname, strlen( objlibname ) );
    ypsError theError;

#pragma exception_handler (ypsHandleBasicException, theError,0,_C2_MH_ESCAPE )
    YPS_TRY(theError, "*", MSGACTION_REMOVE)
    {
	WSY_CHGOWN( objName, "MGTCOL ", newOwner, &revokeValue, result );
    }
# pragma disable_handler
    YPS_CATCH(theError)
    {

    }
    resolveUserProfile( );
    m_sharedReadLock.lockObject( m_objSysPtr );
    
    setCollectionDefaultInterval( defaultInterval );
/**
    for( int x = 0; x < MaxRepositoryNumber; x++)
    {
	m_rpLocalArray[x] = new ypsRepository( );
    }
**/
    setCollectionSystemId( );

    setManagementCollectionObjectType(objAttrStr);          //@D5A
}

void ypsMiCollectionObject::copy(  ypsMiCollectionObject &fromObj )
{

    resolveLibrary( );

    createSpace( );

    m_objBaseSpacePtr = (_SPCPTR) _SETSPPFP( m_objSysPtr ) ;
				   // Lock the space
    lockspace( m_objBaseSpacePtr );
				   // Initialize the composite object header
    m_compositeObjHdr = (ypsStdCompositeObjHdr*)m_objBaseSpacePtr;
    memset( m_compositeObjHdr, 0x00, sizeof( ypsStdCompositeObjHdr ));

    ypsStdCompositeObjHdr tmpCompHdr;
    *m_compositeObjHdr = tmpCompHdr;



				   // Open the from object
    fromObj.open( );
				   // If the from object's number of pointers
				   // is set to the extended value place
				   // the object attributes header beyond 
				   // the space reserved for the pointers
    if( fromObj.m_compositeObjHdr->m_maxRpPointers == MaxRepositoryNumberExt )
    {
	char* tptr
	  = (char*)(m_objBaseSpacePtr) + CompositeObjHdrSizeExt;
	m_colObjAttr = (ypsCollectionAttributes*)tptr;

	ypsCollectionAttributes tmpHdr;
	*m_colObjAttr = tmpHdr;
	m_compositeObjHdr->m_maxRpPointers = MaxRepositoryNumberExt;
    }
				   // Else, place the object attributes header
				   // in it's original location
    else
    {
	char* tptr
	  = (char*)(m_objBaseSpacePtr) +  sizeof(ypsStdCompositeObjHdr) ;
	m_colObjAttr = (ypsCollectionAttributes*)tptr;

	ypsCollectionAttributes tmpHdr;
	*m_colObjAttr = tmpHdr;
	m_compositeObjHdr->m_maxRpPointers = MaxRepositoryNumber;
    }
                                   //@B5A User _MATS to get the from object's
                                   //@B5A base space size.
    _SPC_Template_T matsTpl;
    memset( &matsTpl, '\0', sizeof(_SPC_Template_T));	
    matsTpl.TmpSize = sizeof(_SPC_Template_T);
    _MATS( &matsTpl, &(fromObj.m_objSysPtr) );

    int fromObjSize = matsTpl.Size;


				   // Copy the from object's attributes to
				   // to the new object's attributes
    memcpy( m_colObjAttr, fromObj.m_colObjAttr
	    , sizeof( ypsCollectionAttributes ) );
                                   //@B5A Calculate the offset of the user
                                   //@B5A area pointer.
    int userAreaOffset = stsppo( m_colObjAttr->m_userAreaPtr );
                                   //@B5A Assign userAreaPtr to the 
                                   //@B5A nextAvailAddr
    m_colObjAttr->m_nextAvailAddr = m_colObjAttr->m_userAreaPtr;
                                   //@B5A Now calculate the nextAvailAddr
                                   //@B5A by subtracting the user area offset
                                   //@B5A from the object size
    m_colObjAttr->m_nextAvailAddr += (fromObjSize - userAreaOffset);
				   // Do a byte for byte copy of storage from
				   // the from object's main space to the
				   // new object's main space starting at the
				   // user area pointer.
    memcpy( m_colObjAttr->m_userAreaPtr
	    , fromObj.m_colObjAttr->m_userAreaPtr
                                   //@B5A The size to copy is from the 
                                   //@B5A difference between the size of the
                                   //@B5A from object and the offset to the
                                   //@B5A user area.
	    , (fromObjSize - userAreaOffset)
	    );
                                   // Set misc. values from the from object
    setCollectionDefaultInterval( fromObj.m_colObjAttr->m_defaultInterval );

    memcpy( m_colObjAttr->m_logicalSSN
	    ,  fromObj.m_colObjAttr->m_logicalSSN
	    , sizeof( m_colObjAttr->m_logicalSSN )
	    );

    int fromIsActive = fromObj.isActive( );

    fromObj.resolveUserProfile( );
    m_userProfileSysPtr = fromObj.m_userProfileSysPtr;

				   // Set the first pointer in the composite
				   // object header to itself.
    m_compositeObjHdr->m_rpPointerArray[0]  = m_objSysPtr;
    m_compositeObjHdr->m_numberOfPtrs = 1;
                                   // Loop through each pointer in the from
                                   // object's composite object header
    for( int idx = 1;
	 idx < fromObj.m_compositeObjHdr->m_numberOfPtrs; idx++ )
    {
	ypsRpHandle fromRpPtr = NULL;
	ypsRpHandle toRpPtr   = NULL;

	char fullName[32];
				   // Find the name of the repository bsf
	ypsRepository::resolveName
	  ( fromObj.m_compositeObjHdr->m_rpPointerArray[idx], fullName );
                                   // Determine if the current from object
                                   // repository is a primary repository or
                                   // a secondary.
	ypsRpAssocSpace aSpcFlag;
	if( fullName[29] != 'S' )
	{
	    aSpcFlag = YpsCreateAssocSpc;
	}
	else
	{
	    aSpcFlag = YpsNoAssocSpc;
	}
				   //@C1A Find the ASP number of the library
				   //@C1A the object will be placed into
	_MOBI_Template_T aspNumberTpl;
	uint32 matObjInfoOption = 3;
	memset( &aspNumberTpl, 0, sizeof(aspNumberTpl) );
	_SYSPTR *parmPtr = &m_objLibSysPtr;
	_MATOBJIF( &aspNumberTpl, parmPtr, 3 );
	unsigned short aspNum  = aspNumberTpl.u.Opt3b.ASP_Number;

				   // The current from object repository.
	fromRpPtr = fromObj.m_compositeObjHdr->m_rpPointerArray[idx];
				   // Construct a temporary repository and
				   // do a direct Copy of the from object
				   // repository.
	ypsRepository* toRp = new ypsRepository();
	int gotLock = m_exclusiveLock.lockObject( m_objSysPtr ); //@D9A
	toRpPtr = (*toRp).directCopy( fullName
				      , m_userProfileSysPtr
				      , fromRpPtr
				      , aSpcFlag
                                   //@C1C 
				      , aspNum
				      , gotLock // @D9A
				      ) ;
	m_exclusiveLock.unlockObject( m_objSysPtr ); //@D9A
				   // Assign the newly copied repository ptr
                                   // to the composite object array
	m_compositeObjHdr->m_rpPointerArray[idx] = toRpPtr;
	++m_compositeObjHdr->m_numberOfPtrs;
        delete toRp;
    }
                                   // Unlock the new object.
    unlockspace( m_objBaseSpacePtr );
                                   // Close the from object
    fromObj.close( );
    if( fromIsActive )
    {
	setCollectionEndTime( );
    }
}

void ypsMiCollectionObject::open( )
{
                                   //$A4A Have resolveObject create a external
                                   //$A4A exception if the object doesn't exist
    resolveObject( 1 );

    int mchError = 0;

    ypsError theError;

    if( !m_sharedReadLock.verifyLock( ) )
    {
	mchError = !(m_sharedReadLock.lockObject( m_objSysPtr ));
    }

    if( mchError )
    {
	m_objSysPtr = NULL;

	char msgText[31];
	sprintf( msgText, "%-10.10s%-10.10s%-10.10s"
		 , (char*)m_objName
		 , (char*)m_objLibrary
		 , "MGTCOL"
		 );

	sendError( "CPF2114", msgText, 30 );
    }

    char rtnName[32];
    //========================================================================
    // With the enhancement of secondary repositories in v5r1, going through
    // the array of repository system pointers is a little more complicated. 
    // If there are secondary reposities within this array, they will always
    // occur after their primary repository has been created. Since the first
    // repository in the list will be a primary repository, the local rp
    // array can begin (this array will only contain objects for primary
    // repositories.)  After which, the loop will determine if the name of
    // the repository already exists (primary and secondary repositories have
    // the same name minus the last character) and if a name match occurs
    // then it is assumed to be a secondary repository and handled in that
    // mannner.  If no match occurs then it is assumed to be a primary
    // repository and a new repository object is constructed and placed into
    // the local repository array for the collection object.
    //========================================================================
                                   //$A3A 
    lockspace( m_objBaseSpacePtr );
				   //$A2A test to see if the numberOfPtrs is
				   //$A2A greater than 1, if it is continue
				   //$A2A there are repositories, if not
				   //$A2A do nothing about repositories 
				   //$A2A because there aren't any
    if( m_compositeObjHdr->m_numberOfPtrs > 1 )
    {

	m_colObjAttr->m_rpArray[0].scrubClean( );
				   //@B1A 
				   //@B1A 
	if( m_compositeObjHdr->m_maxRpPointers == MaxRepositoryNumberExt )
	{
	    m_rpLocalArray[0]
	      = new ypsRepository( m_compositeObjHdr->m_rpPointerArray[1] );
	}
	else
	{
	    if (m_colObjAttr->m_nextAvailAddr == NULL)                       //@D7A
	    {                                                                //@D7A
		_SPC_Template_T matsTpl;                                     //@D7A
		memset( &matsTpl, '\0', sizeof(_SPC_Template_T));	     //@D7A
		matsTpl.TmpSize = sizeof(_SPC_Template_T);                   //@D7A
		_MATS( &matsTpl, &(m_objSysPtr) );                           //@D7A
		int objSize = matsTpl.Size;                                  //@D7A
		int userAreaOffset = stsppo( m_colObjAttr->m_userAreaPtr );  //@D7A

		m_colObjAttr->m_nextAvailAddr = m_colObjAttr->m_userAreaPtr; //@D7A
		m_colObjAttr->m_nextAvailAddr += (objSize - userAreaOffset); //@D7A
	    }                                                                //@D7A

	    m_compositeObjHdr->m_maxRpPointers = MaxRepositoryNumber;
	    m_rpLocalArray[0]
	      = new ypsRepository( &m_colObjAttr->m_rpArray[0]
				   , m_compositeObjHdr->m_rpPointerArray[1]
				   , m_objBaseSpacePtr
                                   , &(m_colObjAttr->m_nextAvailAddr) );
	}
	uint32 numPtrs = m_compositeObjHdr->m_numberOfPtrs;

	m_rpLocalIdx = 1;
        int y, idx;
	for(y = 2; y < numPtrs; y++ )
	{
				   // Find the name of the repository bsf
	    ypsRepository::resolveName
	      ( m_compositeObjHdr->m_rpPointerArray[y], rtnName );
				   // Loop through the current array of
				   // local repository objects to find a name
				   // match.
	    for(idx = 0; idx < m_rpLocalIdx; idx++ )
	    {	
				   // If a name match, then it is assumed to
				   // be a secondary repository, initialize
				   // the secondary stuff int the repository
				   // object.
		if( m_rpLocalArray[idx]->nameMatch( rtnName ) )
		{
		    m_rpLocalArray[idx]->setSecondary
		      ( m_compositeObjHdr->m_rpPointerArray[y] );
		    break;
		}
	    }
				   // If the loop index is equal to the
				   // current repository index, then no
				   // match was found, construct another
				   // repository object.
	    if( idx == m_rpLocalIdx )
	    {

		if( m_compositeObjHdr->m_maxRpPointers == MaxRepositoryNumberExt )
		{
		    m_rpLocalArray[idx]
		      = new ypsRepository
		      ( m_compositeObjHdr->m_rpPointerArray[y] );
		}
		else
		{
		    m_colObjAttr->m_rpArray[idx].scrubClean( );

		    m_rpLocalArray[idx]
		      = new ypsRepository
		      ( &m_colObjAttr->m_rpArray[idx]
			, m_compositeObjHdr->m_rpPointerArray[y]
			, m_objBaseSpacePtr
                        , &(m_colObjAttr->m_nextAvailAddr) );
		}

				   // Bump the repository index
		++m_rpLocalIdx ;
	    }

	}
    }
				   //$A3A 
    unlockspace( m_objBaseSpacePtr );


}
                                   //@AAA 
void ypsMiCollectionObject::close( /* int doEnsure */  )
{
                                   //$A4A Have resolveObject create a external
                                   //$A4A exception if the object doesn't exist
    resolveObject( 1 );

                                   //@AAA
/**
    if( doEnsure )
    {
	_ENSOBJ( &m_objSysPtr );
    }
**/
                                   //$A3A 
    lockspace( m_objBaseSpacePtr );

    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
                                   //@B9A Do not go past fixed size
	if( idx < MaxRepositoryNumber )
	{
	    m_colObjAttr->m_rpArray[idx]
	      .ypsv4r4Repository::~ypsv4r4Repository();
	}
    }

                                   //$A3A 
    unlockspace( m_objBaseSpacePtr );
    m_sharedReadLock.unlockObject( m_objSysPtr );
                                   //@BAA Unlock the base object, if this lock is
                                   //@C3 on (set during a openRep for write)
    m_exclusiveReadLock.unlockObject( m_objSysPtr );

}

void ypsMiCollectionObject::destroy( )
{

    char cmdstr[255];
    sprintf( cmdstr, "QSYS/DLTMGTCOL MGTCOL(%s/%s)"
	     , (char*)m_objLibrary
	     , (char*)m_objName
	     );
    int rc = system( cmdstr );
}


void ypsMiCollectionObject::resolveLibrary( )
{
    if( m_objLibSysPtr == NULL )
    {
                                   //@B6A If the library is QTEMP then the
                                   //@B6A System pointer resides in the WCB
                                   //@B6A since QTEMP is a 'special' library
	if( m_objLibrary == "QTEMP" )
	{
	    wwcb1 *wcbp;
	    GETWCB(wcbp);
	    EXTWCB(wcbp, WWCBJTLB, SYS_PTR, m_objLibSysPtr );
	}
	else
	{

	    _RSLV_Template_T rslvsp_tmpl;

	    memset( &rslvsp_tmpl, ' ', sizeof( _RSLV_Template_T ));
	    memset( rslvsp_tmpl.Obj.Name ,' '    , 30              );

	    memcpy( rslvsp_tmpl.Obj.Name, (char*)m_objLibrary, m_objLibrary.size() );

	    rslvsp_tmpl.Obj.Type_Subtype = WLI_LIB; 
	    rslvsp_tmpl.Auth             = _AUTH_NONE;
	    ypsError theError;
#pragma exception_handler (ypsHandleBasicException, theError,0,_C2_MH_ESCAPE)
	    YPS_TRY(theError, "MCH3401", MSGACTION_REMOVE)
	    {
		_RSLVSP2( &m_objLibSysPtr, &rslvsp_tmpl );
	    }
# pragma disable_handler
	    YPS_CATCH(theError)
	    {
		m_objLibSysPtr = NULL;
	    }

	    if( m_objLibSysPtr == NULL )
	    {
		sendError( "CPF2110", rslvsp_tmpl.Obj.Name, 10 );
	    }
	}
    }
}

void ypsMiCollectionObject::resolveUserProfile(  )
{

    if( !m_userProfileSysPtr )
    {
	materializeSystemObject( );

	_RSLV_Template_T rslvsp_tmpl;

	memset( &rslvsp_tmpl, 0, sizeof( _RSLV_Template_T ));
	memset( rslvsp_tmpl.Obj.Name ,' '    , 30              );

	memcpy( rslvsp_tmpl.Obj.Name
		, m_matObjTpl.User_Profile.Name
		, sizeof( m_matObjTpl.User_Profile.Name)
		);

	rslvsp_tmpl.Obj.Type_Subtype = WLI_USRPRF; 
	rslvsp_tmpl.Auth = _AUTH_NONE;

	_RSLVSP2( &m_userProfileSysPtr
		  , &rslvsp_tmpl
		  );
    }
}

                                   //$A4A Added a flag to determine whether to
                                   //$A4A throw a external exception.
int ypsMiCollectionObject::resolveObject( int externalException )
{

    if( m_objSysPtr == NULL )
    {
	_RSLV_Template_T rslvsp_tmpl;

	memset( &rslvsp_tmpl, ' ', sizeof( _RSLV_Template_T ));
	memcpy( rslvsp_tmpl.Obj.Name, (char*)m_objName, m_objName.size() );

	rslvsp_tmpl.Obj.Type_Subtype = WLI_MGTCOL; // WLI_MGTCOL;
	rslvsp_tmpl.Auth = _AUTH_NONE;

	if( m_objLibSysPtr == NULL )
	{
	    resolveLibrary( );
	}

	ypsError theError;
#pragma exception_handler (ypsHandleBasicException, theError,0,_C2_MH_ESCAPE)
	YPS_TRY(theError, "*", MSGACTION_REMOVE)
	{
	    _RSLVSP4( &m_objSysPtr, &rslvsp_tmpl, &m_objLibSysPtr   );
	}
# pragma disable_handler
	YPS_CATCH(theError)
	{
	    m_objSysPtr = NULL;
	}     

	if ( m_objSysPtr == NULL )
	{
				   //$A4A If no external exception then return
				   //$A4A 0, object does not exist.
	    if(!externalException)
	    {
		return 0;
	    }

	    char rplText[50];
	    sprintf( rplText, "%-10.10s%-10.10s%-10.10s"
		     , (char*)m_objName
		     , (char*)m_objLibrary
				   //$A4A This was in error, now the string is
                                   //$A4A properly formed.
		     , "MGTCOL"
		     );
	    sendError( "CPF2105", rplText, strlen(rplText) );
	}
    }
    m_objBaseSpacePtr = (_SPCPTR) _SETSPPFP( m_objSysPtr ) ;
    m_compositeObjHdr = (ypsStdCompositeObjHdr*)m_objBaseSpacePtr;

    if( m_compositeObjHdr->m_maxRpPointers == MaxRepositoryNumberExt )
    {
	char* tptr
	  = (char*)(m_objBaseSpacePtr) + CompositeObjHdrSizeExt;
	m_colObjAttr = (ypsCollectionAttributes*)tptr;


    }
    else
    {
	char* tptr
	  = (char*)(m_objBaseSpacePtr) +  sizeof(ypsStdCompositeObjHdr) ;
	m_colObjAttr = (ypsCollectionAttributes*)tptr;
	m_compositeObjHdr->m_maxRpPointers = MaxRepositoryNumber;
    }
                                   //$A4A If reached this point the object does
                                   //$A4A exist, return 1
    return 1;
}
void ypsMiCollectionObject::materializeSystemObject( )
{

    memset( &m_matObjTpl, '\0', sizeof( _MSOB_Template_T ) );
    m_matObjTpl.Template_Size = sizeof( _MSOB_Template_T );


    _MATSOBJ( &m_matObjTpl, &m_objSysPtr );


}
char*
ypsMiCollectionObject::getNewRecord( ypsRpHandle rpHandleId
				     , char* twoByteRecordId
				     , char* eightByteGenericKey
				     , int varDataLength
				     , void* varData
				     , int requestedLength
				     , int &actualLength
				     , YpsRpClearAheadType  clearType 
				     )
{
    uint64 tActLen = (uint64)actualLength;

    char* tPtr =  getNewRecord( rpHandleId
		  , twoByteRecordId
		  , eightByteGenericKey
		  , varDataLength
		  , varData
		  , (uint64)requestedLength
		  , tActLen
		  , clearType
		  );
    actualLength = tActLen;
    return tPtr;
}
char*
ypsMiCollectionObject::getNewRecord( ypsRpHandle rpHandleId
				     , char* twoByteRecordId
				     , char* eightByteGenericKey
				     , int varDataLength
				     , void* varData
				     , uint64 requestedLength
				     , uint64 &actualLength
				     , YpsRpClearAheadType  clearType 
				     )
{

    char *rtnPtr = NULL;

    if( requestedLength > MaxSetacstBlockSize )
	actualLength = MaxSetacstBlockSize - PageLength;
    else
	actualLength = requestedLength;

    int pageAlign = actualLength % PageLength;

    actualLength += (PageLength - pageAlign);
                                   //$A4A Make sure that its one extra page
                                   //$A4A than necessary
    actualLength += PageLength;

    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandleId ) )
	{
	    if( m_rpLocalArray[idx]->whichRepository( eightByteGenericKey )
		== YpsRpPrimary )
	    {
		rtnPtr =  m_rpLocalArray[idx]->getNewBuffer( actualLength
							 , clearType
							 , twoByteRecordId
							 , eightByteGenericKey
							 , (char*)varData
							 , varDataLength
							 );
	    }
	    else
	    {
		createSecondaryRepository( rpHandleId );
		openSecondaryRepository( rpHandleId, ypsRepositoryEnd );
		rtnPtr =  m_rpLocalArray[idx]->getNewSecondaryBuffer
		  ( actualLength
		    , clearType
		    , twoByteRecordId
		    , eightByteGenericKey
		    , (char*)varData
		    , varDataLength
		    );
		
	    }
	    break;
	}
	    
    }
    ypsRpRecordHeader* rpHdrPtr = (ypsRpRecordHeader*)rtnPtr;
    rtnPtr += rpHdrPtr->m_segLen;

    actualLength -= rpHdrPtr->m_segLen;

    return( rtnPtr );
}


char*
ypsMiCollectionObject::extendRecord( ypsRpHandle rpHandleId
				     , int requestedLength
				     , int &actualLength
				     , YpsRpClearAheadType  clearType
				     )
{
    uint64 tActLen = (uint64)actualLength;

    char* tPtr = extendRecord( rpHandleId
			 , (uint64)requestedLength
			 , tActLen
			 , clearType
			 );
    actualLength = tActLen;
    return tPtr;
}


char*
ypsMiCollectionObject::extendRecord( ypsRpHandle rpHandleId
				     , uint64 requestedLength
				     , uint64 &actualLength
				     , YpsRpClearAheadType  clearType
				     )
{

    char* tmpPtr = NULL;

    if( requestedLength > MaxSetacstBlockSize )
	actualLength = MaxSetacstBlockSize - PageLength ;
    else
	actualLength = requestedLength;

    int pageAlign = actualLength % PageLength;

    actualLength += (PageLength - pageAlign);
                                   //$A4A Make sure that its one extra page
                                   //$A4A than necessary
    actualLength += PageLength;

    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandleId ) )
	{
	    tmpPtr = 
	      m_rpLocalArray[idx]->extendBuffer
	      ( actualLength, clearType );

	    break;
	}
	    
    }

    return( tmpPtr );
}

void
ypsMiCollectionObject::ensureRecord( ypsRpHandle rpHandleId
					   , int ensureLength
				     , char *eightByteGenericKey 
				     , void* varData
				     )
{
    
    ensureRecord( rpHandleId
		  , (uint64)ensureLength
		  , eightByteGenericKey
		  , varData
		  );
}
void
ypsMiCollectionObject::ensureRecord( ypsRpHandle rpHandleId
					   , uint64 ensureLength
				     , char *eightByteGenericKey 
				     , void* varData
				     )
{
    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandleId ) )
	{

	    if( m_rpLocalArray[idx]->whichRepository( eightByteGenericKey )
		== YpsRpPrimary )
	    {

		m_rpLocalArray[idx]->ensureBuffer( ensureLength
						   , eightByteGenericKey
						   , varData
						   );
	    }
	    else
	    {

		m_rpLocalArray[idx]->ensureSecondaryBuffer
		  ( ensureLength
		    , eightByteGenericKey
		    , varData
		    );
	    }
	    break;
	}
    }
}


void
ypsMiCollectionObject::ensurePartialRecord(  ypsRpHandle rpHandleId
					     , void* startAddress
					     , int ensureLength
					     )
{
    ensurePartialRecord( rpHandleId, startAddress, (uint64)ensureLength );
}
void
ypsMiCollectionObject::ensurePartialRecord(  ypsRpHandle rpHandleId
					     , void* startAddress
					     , uint64 ensureLength
					     )
{
    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandleId ) )
	{
	    m_rpLocalArray[idx]->
	      ensurePartialBuffer( startAddress, ensureLength );

	    break;
	}
    }
}

ypsRpHandle
ypsMiCollectionObject::createRepository
( char* tenByteName
//, ypsRpErrorCode &rpErrorCode
)
{
    ypsRpErrorCode rpErrorCode;
    char fullName[31];
    sprintf (fullName, "%-30.30s"
	     , tenByteName
	     );

    // @D3A There is a possibility that the local copy of repository
    // objects may be out of sync from the array stored within the 
    // collection object itself. The following code will update the 
    // local repository array with the appropriate information if
    // needed.

    ypsLock lock( m_objMutex );						// @D3A
    lockspace( m_objBaseSpacePtr );					// @D3A

    ypsMiCollectionObject tmpObj( m_objLibrary, m_objName );		// @D3A
    tmpObj.open( );							// @D3A

    if( m_rpLocalIdx < tmpObj.m_rpLocalIdx )				// @D3A
    {									// @D3A
	for( int x = m_rpLocalIdx; x < tmpObj.m_rpLocalIdx ; x++)	// @D3A
	{								// @D3A
	    if( m_rpLocalArray[x] )					// @D3A
	    {								// @D3A
		delete m_rpLocalArray[x];				// @D3A
	    }								// @D3A
	    m_rpLocalArray[x] = new ypsRepository( );			// @D3A
	    *m_rpLocalArray[x] = *tmpObj.m_rpLocalArray[x];		// @D3A
	}								// @D3A
	m_rpLocalIdx =  tmpObj.m_rpLocalIdx;				// @D3A
    }									// @D3A
    tmpObj.close( );							// @D3A

    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->nameMatch( fullName ) )
	{
				   // Repository already exists
				   // return a NULL repository handle
	    unlockspace(m_objBaseSpacePtr);				// @D3A
	    return NULL;
	    break;
	}
    }

    resolveUserProfile( );

    int size = 0;
    ypsRpHandle rpPtr = NULL;
                                   //@C1A Find the ASP number of the library
                                   //@C1A the object will be placed into
    _MOBI_Template_T aspNumberTpl;
    uint32 matObjInfoOption = 3;
    memset( &aspNumberTpl, 0, sizeof(aspNumberTpl) );
    _SYSPTR *parmPtr = &m_objLibSysPtr;
    _MATOBJIF( &aspNumberTpl, parmPtr, 3 );
    unsigned short aspNum = aspNumberTpl.u.Opt3b.ASP_Number;
                                   //@C4 Find out the attribute of the MGTCOL
                                   //@C4 object
    retrieveObjectDesc( );
    int mgtcolType = ypsRpAny;
                                   //@C4 If a pex object set type to pex
    if( memcmp(m_objDesc.Extended_Obj_Attr, "*PEX", 4 ) == 0 )
    {
	mgtcolType = ypsRpPex;
    }
                                   //@C4 If pfr object set type to pfr
    else if( memcmp(m_objDesc.Extended_Obj_Attr, "*PFR ", 5 ) == 0 )
    {
	mgtcolType = ypsRpPfr;
    }

    if( m_compositeObjHdr->m_numberOfPtrs <
	m_compositeObjHdr->m_maxRpPointers )
    {
	m_rpLocalArray[m_rpLocalIdx] = new ypsRepository( );
                                   //@B1A 
                                   //@B1A 
	if( m_compositeObjHdr->m_maxRpPointers == MaxRepositoryNumberExt )
	{
	    rpPtr = 
	      m_rpLocalArray[m_rpLocalIdx]
	      ->create( fullName
			, m_userProfileSysPtr
			, NULL
			, size
			, rpErrorCode
                                   //@B4A Pex Only Repository
			, mgtcolType
                                   //@C1C 
			, aspNum
			) ;

	}

	else
	{
	    rpPtr = 
	      m_rpLocalArray[m_rpLocalIdx]
	      ->create( fullName
			, m_userProfileSysPtr
			, m_colObjAttr-> m_nextAvailAddr
			, size
			, rpErrorCode
			, 0
                                   //@C1C 
			, aspNum
			) ;

	}

	m_compositeObjHdr->m_rpPointerArray
	  [m_compositeObjHdr->m_numberOfPtrs] = rpPtr;

	m_colObjAttr->m_nextAvailAddr += size;

	++m_compositeObjHdr->m_numberOfPtrs; 
	++m_colObjAttr->m_repositoryCount;
	++m_rpLocalIdx;

	openRepository( fullName
			, ypsRepositoryStart
			, YpsRpWriteOnly
			, 0
			, YpsHsMapYes	
		);
    }
                                   //$A3A 
    unlockspace( m_objBaseSpacePtr );

    return( rpPtr );
}

int
ypsMiCollectionObject::createSecondaryRepository
( ypsRpHandle rpHandle )
{
    ypsRpErrorCode rpErrorCode;
    _SYSPTR rpPtr = NULL;

    resolveUserProfile( );
                                   //@C1A Find the ASP number of the library
                                   //@C1A the object will be placed into
   _MOBI_Template_T aspNumberTpl;
    uint32 matObjInfoOption = 3;
    memset( &aspNumberTpl, 0, sizeof(aspNumberTpl) );
    _SYSPTR *parmPtr = &m_objLibSysPtr;
    _MATOBJIF( &aspNumberTpl, parmPtr, 3 );
    unsigned short aspNum = aspNumberTpl.u.Opt3b.ASP_Number;

    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandle ) )
	{
	    rpPtr = 
	      m_rpLocalArray[idx]
                                   //@C1C 
	      ->createSecondary(  m_userProfileSysPtr, rpErrorCode, aspNum ) ;
                                   // If a valid pointer is returned, then
                                   // a secondary repository was created.
                                   // If NULL then one was already created.

	    if( rpPtr )
	    {

				   //$A3A 
		lockspace( m_objBaseSpacePtr );
		m_compositeObjHdr->m_rpPointerArray
		  [m_compositeObjHdr->m_numberOfPtrs] = rpPtr;

		++m_compositeObjHdr->m_numberOfPtrs;
				   //$A3A 
		unlockspace( m_objBaseSpacePtr );

	    }
	    break;
	}
    }
    return( rpPtr != NULL );

}

ypsRpHandle
ypsMiCollectionObject::openRepository( char* repositoryName
				       , ypsRepositoryOpen openPosition
				       , int openType
				       , int newInterval
				       , ypsRpHsMap hsMapOption 
//				       , ypsRpErrorCode &rpErrorCode
				       )
{
    ypsRpErrorCode rpErrorCode;
    char fullName[31];
    ypsRpHandle localPtr = NULL;
    sprintf (fullName, "%-30.30s"
	     , repositoryName
	     , (char*)m_objName
	     , (char*)m_objLibrary
	     );

                                   //@BAA If an open repostiory write only request
                                   //@BAA is made, lock the main object with a
                                   //@BAA _LSUP_LOCK lock
                                   //@D4C Don't need this lock for PEX mgtcols
    YpsMgtcolObjectType type = getManagementCollectionObjectType(); // @D4A @D5C
    if( openType > YpsRpReadOnly && type != YpsMgtcolTypePEX) // @D4C @D5C
    {
                                   //@BAA Lock object ensures that only one lock per
                                   //@BAA process is obtained.
	m_exclusiveReadLock.lockObject( m_objSysPtr );
    }	

                                   //$A2A There is a possibility that the
                                   //$A2A local copy of repository objects
                                   //$A2A may be out of date from the actual
                                   //$A2A col object. This code will determine
                                   //$A2A if that is the case and update the
                                   //$A2A local repository array with the 
                                   //$A2A appropriate information.
                                   //$A3A 
    lockspace( m_objBaseSpacePtr );

    ypsMiCollectionObject tmpObj( m_objLibrary, m_objName );
    tmpObj.open( );
                                   //@BBA If the current local index is less than the
                                   //@BBA tmpObj's local index then add any new
                                   //@BBA repositories.
    if( m_rpLocalIdx < tmpObj.m_rpLocalIdx )
    {
	for( int x = m_rpLocalIdx
	     ; x < tmpObj.m_rpLocalIdx ; x++)
	{
	    if( m_rpLocalArray[x] )
	    {
		delete m_rpLocalArray[x];
	    }
                                   //@BBA Moved assignment statement outside the if
                                   //@BBA statemment, Create a new ypsRepository
                                   //@BBA and copy the repository from the tmpObj.
	    m_rpLocalArray[x] = new ypsRepository( );
	    *m_rpLocalArray[x] = *tmpObj.m_rpLocalArray[x];

	}

    }
                                   //@BBA Set local index to tmpObj's local index
    m_rpLocalIdx =  tmpObj.m_rpLocalIdx;
                                   //@BBA Close tmpObj
    tmpObj.close( );

    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->nameMatch( fullName ) )
	{
	    
	    localPtr = m_rpLocalArray[idx]
	      ->open( openPosition
		     , (ypsRpOpenType)openType
		     , rpErrorCode
		     , newInterval
		     , hsMapOption
		     );
	    if( m_compositeObjHdr->m_maxRpPointers != MaxRepositoryNumberExt )
	    {
                                   //@B9A Do not go past fixed size
		if( idx < MaxRepositoryNumber )
		{
		    m_colObjAttr->m_rpArray[idx] = *m_rpLocalArray[idx];
		}
                                   //@B8A
		syncstg (SYNC_WRITESONLY);

	    }
                                   //$B0A Deleted unneeded call to 
                                   //$B0A openSecondaryRepository
	    break;
	}
    }
                                   //$A3A 
    unlockspace( m_objBaseSpacePtr );

    return( localPtr );
}

int
ypsMiCollectionObject::openSecondaryRepository
( ypsRpHandle rpHandle, ypsRepositoryOpen openPosition )
{
    ypsRpErrorCode rpErrorCode;
    int theRc = 0;
    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandle ) )
	{
	    theRc = 
	      m_rpLocalArray[idx]
	      ->openSecondary( openPosition
			       , YpsRpWriteOnly
			       , rpErrorCode
			       );

	    break;
	}
    }
    return( theRc );

}



void ypsMiCollectionObject::closeRepository
( ypsRpHandle rpHandleId, ypsRpHsCursor *recordCursor )
{
    ypsLock lock( m_objMutex );
    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandleId ) )
	{
	    int gotLock = m_exclusiveLock.lockObject( m_objSysPtr );	//@D9A
	    m_rpLocalArray[idx]->close( recordCursor, gotLock );	//@D9C
	    m_exclusiveLock.unlockObject( m_objSysPtr );		//@D9A
	    
				   //$A3A 
	    lockspace( m_objBaseSpacePtr );
                                   //@B9A Do not go past fixed size
	    if( idx < MaxRepositoryNumber )
	    {
		m_colObjAttr->m_rpArray[idx] = *m_rpLocalArray[idx];
	    }
				   //$A3A 
	    unlockspace( m_objBaseSpacePtr );
	    break;
	}
    }
                                   //@BAA Unlock the base object
                                   //@C3 DO NOT unlock on a close repoistory
                                   //@C3 because if just one out of x is closed
                                   //@C3 the object becomes eligible for SWA
    //@C3 m_exclusiveReadLock.unlockObject( m_objSysPtr );


}

ypsRpRecordHeader*
ypsMiCollectionObject::readFirstRecord( ypsRpHandle rpHandleId
					, char *twoByteRecordId
					, ypsRpHsCursor *recordCursor
					, int &bytesReadInMemory
					, int &bytesRemainingToRead
//					, ypsRpErrorCode &rpErrorCode

)
{
    ypsRpErrorCode rpErrorCode;
    uint64 tReadIn = (uint64)bytesReadInMemory;
    uint64 tRemain = (uint64)bytesRemainingToRead;
    ypsRpRecordHeader * rPtr= readFirstRecord( rpHandleId
					       , twoByteRecordId
					       , recordCursor
					       , tReadIn
					       , tRemain
//					       , rpErrorCode
					       );
    bytesReadInMemory = tReadIn;
    bytesRemainingToRead = tRemain;

    return rPtr;
}

ypsRpRecordHeader*
ypsMiCollectionObject::readFirstRecord( ypsRpHandle rpHandleId
					, char *twoByteRecordId
					, ypsRpHsCursor *recordCursor
					, uint64 &bytesReadInMemory
					, uint64 &bytesRemainingToRead
//					, ypsRpErrorCode &rpErrorCode

)
{
    ypsRpErrorCode rpErrorCode;
    ypsRpRecordHeader *localHdrPtr = NULL;
    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandleId ) )
	{
	    m_rpLocalArray[idx]
	      ->getFirstRecord( twoByteRecordId
			       , recordCursor
			       , bytesReadInMemory
			       , bytesRemainingToRead
			       , rpErrorCode 
			      );
	    localHdrPtr = (ypsRpRecordHeader*)recordCursor->m_hsCurAddr;
	    break;
	}
    }
    return localHdrPtr;
}

ypsRpRecordHeader*
ypsMiCollectionObject::readNextRecord( ypsRpHandle rpHandleId
				       , char *twoByteRecordId
				       , ypsRpHsCursor *recordCursor
				       , int &bytesReadInMemory
				       , int &bytesRemainingToRead
//				       , ypsRpErrorCode &rpErrorCode

)
{
    ypsRpErrorCode rpErrorCode;
    uint64 tReadIn = (uint64)bytesReadInMemory;
    uint64 tRemain = (uint64)bytesRemainingToRead;
    ypsRpRecordHeader * rPtr= readNextRecord( rpHandleId
					       , twoByteRecordId
					       , recordCursor
					       , tReadIn
					       , tRemain
//					       , rpErrorCode
					       );
    bytesReadInMemory = tReadIn;
    bytesRemainingToRead = tRemain;

    return rPtr;
    
}
ypsRpRecordHeader*
ypsMiCollectionObject::readNextRecord( ypsRpHandle rpHandleId
				       , char *twoByteRecordId
				       , ypsRpHsCursor *recordCursor
				       , uint64 &bytesReadInMemory
				       , uint64 &bytesRemainingToRead
//				       , ypsRpErrorCode &rpErrorCode

)
{
    ypsRpErrorCode rpErrorCode;
    ypsRpRecordHeader *localHdrPtr = NULL;
    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandleId ) )
	{
	    m_rpLocalArray[idx]
	      ->getNextRecord( twoByteRecordId
			      , recordCursor
			      , bytesReadInMemory
			      , bytesRemainingToRead
			      , rpErrorCode
			      );
	    localHdrPtr = (ypsRpRecordHeader*)recordCursor->m_hsCurAddr;

	    break;
	}
    }
    return localHdrPtr;
}

ypsRpRecordHeader*
ypsMiCollectionObject::readRecordByKey( ypsRpHandle rpHandleId
					, char *twoByteRecordId
					, char* eightByteKey
					, ypsRpHsCursor *recordCursor
					, int &bytesReadInMemory
					, int &bytesRemainingToRead
					, ypsReadKeyOptions keyOptions
//					, ypsRpErrorCode &rpErrorCode
					)
{
    uint64 tReadIn = (uint64)bytesReadInMemory;
    uint64 tRemain = (uint64)bytesRemainingToRead;
    ypsRpRecordHeader * rPtr= readRecordByKey( rpHandleId
					       , twoByteRecordId
					       , eightByteKey
					       , recordCursor
					       , tReadIn
					       , tRemain
					       , keyOptions
//					       , rpErrorCode
					       );
    bytesReadInMemory = tReadIn;
    bytesRemainingToRead = tRemain;

    return rPtr;
}
ypsRpRecordHeader*
ypsMiCollectionObject::readRecordByKey( ypsRpHandle rpHandleId
					, char *twoByteRecordId
					, char* eightByteKey
					, ypsRpHsCursor *recordCursor
					, uint64 &bytesReadInMemory
					, uint64 &bytesRemainingToRead
					, ypsReadKeyOptions keyOptions
//					, ypsRpErrorCode &rpErrorCode
					)
{
    ypsRpErrorCode rpErrorCode;
    ypsRpRecordHeader *localHdrPtr = NULL;
    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandleId ) )
	{
	    m_rpLocalArray[idx]
	      ->getRecordByKey( twoByteRecordId
			       , eightByteKey
			       , recordCursor
			       , bytesReadInMemory
			       , bytesRemainingToRead
			       , keyOptions
			       , rpErrorCode
			       );

	    localHdrPtr = (ypsRpRecordHeader*)recordCursor->m_hsCurAddr;
	    break;
	}
    }
    return localHdrPtr;
}

char*
ypsMiCollectionObject::readRepositoryAtAddress( ypsRpHandle  rpHandleId
						, ypsRpHsCursor *recordCursor
						, uint64 bytesRequested
						, uint64 &bytesReturned
						)
{
    char* localPtr = NULL;
    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandleId ) )
	{
	    m_rpLocalArray[idx]
	      ->getAtAddress( recordCursor
			     , bytesRequested
			     , bytesReturned
			     );
	    localPtr = recordCursor->m_hsCurAddr;

	    break;
	}
    }
    return localPtr;

}

char*
ypsMiCollectionObject::readRepositoryAtOffset
( ypsRpHandle  rpHandleId
  , ypsRpHsCursor *recordCursor
  , uint64 bsoOffset
  , uint64 bytesRequested
  , uint64 &bytesReturned
//  , ypsRpErrorCode &rpErrorCode
  )
{
    ypsRpErrorCode rpErrorCode;

    char* localPtr = NULL;
    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandleId ) )
	{
	    m_rpLocalArray[idx]
	      ->getAtOffset( recordCursor
			    , bsoOffset
			    , bytesRequested
			    , bytesReturned
			    , rpErrorCode
			     );
	    localPtr = recordCursor->m_hsCurAddr;

	    break;
	}
    }
    return localPtr;

}

char* ypsMiCollectionObject::allocateFromBaseSpace( int userAreaSize )
{
    uint32 alignAdjust = userAreaSize % 16;
    if( alignAdjust )
    {
	alignAdjust = 16 - alignAdjust;
    }
    userAreaSize += alignAdjust;

                                   //$A3A 
    lockspace( m_objBaseSpacePtr );
    m_colObjAttr->m_userAreaSize = userAreaSize;
    m_colObjAttr->m_totalSizeBase = userAreaSize;
    m_colObjAttr->m_nextAvailAddr
      = m_colObjAttr->m_userAreaPtr + userAreaSize;
                                   //$A3A 
    unlockspace( m_objBaseSpacePtr );

    return( m_colObjAttr->m_userAreaPtr );

}
char* ypsMiCollectionObject::accessBaseSpaceArea(  )
{
    char* rtnPtr = NULL;
				   //$A3A 
    lockspace( m_objBaseSpacePtr );

    if( m_colObjAttr->m_userAreaSize > 0 )
    {
	rtnPtr =  m_colObjAttr->m_userAreaPtr ;
    }
				   //$A3A 
    unlockspace( m_objBaseSpacePtr );
    return rtnPtr;
}

void ypsMiCollectionObject::setRetentionHours( int numberOfHours )
{
                                   //$A3A 
    lockspace( m_objBaseSpacePtr );
    m_colObjAttr->m_objRetentionHours = numberOfHours;
                                   //$A3A 
    unlockspace( m_objBaseSpacePtr );


}

int ypsMiCollectionObject::getRetentionHours(  )
{
    int rtnInt = 0;
                                   //$A3A 
    lockspace( m_objBaseSpacePtr );
    rtnInt = m_colObjAttr->m_objRetentionHours ;
                                   //$A3A 
    unlockspace( m_objBaseSpacePtr );

    return rtnInt;
}

void ypsMiCollectionObject::setCollectionStartTime
( ypsUniversalTime *startTime )
{
    ypsUniversalTime theTime;

    lockspace( m_objBaseSpacePtr );

				   //@C0A Materialize mi tod and initialize
				   //@C0A univeral time based on that.
    if( startTime == NULL )
    {
        // mattod is in local time.
	mattod( (char*)&m_colObjAttr->m_miTodStart );
	ypsUniversalTime todTime( (char *)&m_colObjAttr->m_miTodStart );
	theTime = todTime;
    }
    else if( startTime )
    {
	theTime = *startTime;
	m_colObjAttr->m_miTodStart = theTime.getMiTodLocalTime( );
    }

    // @D0C The time is now in Lilian time which is seconds since
    // October 14, 1582, so for backwards compatibility, we have to
    // subract the difference in seconds between October 14, 1582 and
    // January 1, 1970.
    long_time_t time = theTime.getTime( );
    if (time != 0)
        time = time - CTIME_OFFSET;

    if (time >= (long_time_t)INT_MAX)
        m_colObjAttr->m_collectionStart = (time_t)INT_MAX -1;
    else if (time <= 0)
        m_colObjAttr->m_collectionStart = 0;
    else 
        m_colObjAttr->m_collectionStart = (time_t)time;
    // @D0A set the new 8 byte length time too
    m_colObjAttr->m_collectionStart_long = theTime.getTime();

    ypsString timestamp;
 
    theTime.getLocalTime( timestamp );

    memset( m_colObjAttr->m_startTimestamp
	    , 0
	    , sizeof( m_colObjAttr->m_startTimestamp )
	    );

                                   //@B8A Ensure that the size copied is not
                                   //@B8A too large
    int cpysize = sizeof( m_colObjAttr->m_startTimestamp );
    if( cpysize > timestamp.size( ) )
    {
	cpysize = timestamp.size( );
    }
    memcpy( m_colObjAttr->m_startTimestamp
	    , (char*)timestamp
	    , cpysize
	    );

    m_colObjAttr->m_isActiveFlag = 1;

                                   //$A3A 
    unlockspace( m_objBaseSpacePtr );

//  m_exclusiveReadLock.lockObject( m_objSysPtr );

    _ENSOBJ( &m_objSysPtr );

}

void ypsMiCollectionObject::setCollectionEndTime
( ypsUniversalTime *endtTime, ypsSetEndTime endTimeType )
{
    if( endTimeType == YpsFinal )
    {
                                   //@B8A Synch/lock the storage
	lockspace( m_objBaseSpacePtr );
	m_colObjAttr->m_isActiveFlag  = 0;
                                   //@B8A unlock the storage
	unlockspace( m_objBaseSpacePtr );

    }
    ypsUniversalTime theTime;

    lockspace( m_objBaseSpacePtr );

				   //@C0A Materialize mi tod and initialize
				   //@C0A univeral time based on that.
    if( endtTime == NULL )
    {
	mattod( (char*)&m_colObjAttr->m_miTodEnd );
	ypsUniversalTime todTime( (char *)&m_colObjAttr->m_miTodEnd );
	theTime = todTime;
    }
    else if( endtTime )
    {
	theTime = *endtTime;
	m_colObjAttr->m_miTodEnd = theTime.getMiTodLocalTime( );

    }

    // @D0C The time is now in Lilian time which is seconds since
    // October 14, 1582, so for backwards compatibility, we have to
    // subract the difference in seconds between October 14, 1582 and
    // January 1, 1970.
    long_time_t time = theTime.getTime( );
    if (time != 0)
        time = time - CTIME_OFFSET;

    if (time >= (long_time_t)INT_MAX)
        m_colObjAttr->m_collectionEnd = (time_t)INT_MAX - 1;
    else if (time <= 0)
        m_colObjAttr->m_collectionEnd = 0;
    else
        m_colObjAttr->m_collectionEnd = (time_t)time;
    // @D0A set the new 8 byte length time too
    m_colObjAttr->m_collectionEnd_long = theTime.getTime();

    ypsString timestamp;
 
    theTime.getLocalTime( timestamp );

    memset( m_colObjAttr->m_endTimestamp
	    , 0
	    , sizeof( m_colObjAttr->m_endTimestamp )
	    );

                                   //@B8A Ensure that the size copied is not
                                   //@B8A too large
    int cpysize = sizeof( m_colObjAttr->m_endTimestamp );
    if( cpysize > timestamp.size( ) )
    {
	cpysize = timestamp.size( );
    }
    memcpy( m_colObjAttr->m_endTimestamp
	    , (char*)timestamp
	    , cpysize
	    );

                                   //$A3A 
    unlockspace( m_objBaseSpacePtr );
/**
    if( m_exclusiveReadLock.verifyLock( ) )
    {
	m_exclusiveReadLock.unlockObject( m_objSysPtr );
    }
**/
}

void ypsMiCollectionObject::fixCollectionEndAttributes( )
{
                                   //@B8A Do some initial setup with the
                                   //@B8A space locked/synched.
    lockspace( m_objBaseSpacePtr );

    m_colObjAttr->m_fixEndFlag = 1;
    m_colObjAttr->m_isActiveFlag  = 0;
                                   //@B8A Determine if the end time has been
                                   //@B8A set.
    int endTimeNotSet = (m_colObjAttr->m_collectionEnd == 0);
                                   //@B8A Set the temp end time to the start
                                   //@B8A Time.
    long_time_t endTime_t = m_colObjAttr->m_collectionStart_long;
                                   //@B8A Unlock the space
    unlockspace( m_objBaseSpacePtr );

                                   //@B8A If the end time has not been set
                                   //@B8A Attempt to set it.
    if( endTimeNotSet )
    {
	ypsRpPacketableCollection *rpColl = listRepositoryAttributes( );

	if( rpColl == NULL )
	{
	    return;
	}
	ypsPacketable *packtbl;

	rpColl->getFirst( packtbl );

	ypsRepositoryAttributes *rpAttr;

	rpAttr = (ypsRepositoryAttributes*)packtbl;

                                   //@B8A Set tmpTime to the first rp end
                                   //@B8A timestamp.
	long_time_t tmpTime_t = rpAttr->m_endTimestamp.getTime( );
                                   //@B8A if the rp time is greater than the
                                   //@B8A current end time, make the asignment
	if( tmpTime_t > endTime_t )
	{
	    endTime_t = tmpTime_t;
	}
                                   //@B8A Loop thru the rest of the rp times
	while( rpColl->getNext( packtbl ) == 0 ) 
	{
	    rpAttr = (ypsRepositoryAttributes*)packtbl;
	    tmpTime_t = rpAttr->m_endTimestamp.getTime( );
                                   //@B8A if the rp time is greater than the
                                   //@B8A current end time, make the asignment
	    if( tmpTime_t > endTime_t )
	    {
		endTime_t = tmpTime_t;
	    }
	}
                                   //@B8A Create a univeral time object
	ypsUniversalTime aTime;
	aTime.setTime( endTime_t );
                                   //@B8A call setCollectionEndTime
	setCollectionEndTime( &aTime, YpsFinal );

    }
}

void ypsMiCollectionObject::getCollectionStartTime
(  ypsUniversalTime &startTime )
{
    lockspace( m_objBaseSpacePtr );
    // @D0A begin  Use the long_time_t field now instead if it is set
    if( m_colObjAttr->m_collectionStart_long > 0)
    {
	startTime.setTime (m_colObjAttr->m_collectionStart_long);
    }
    // @D0A end
    // @D0D begin
    //else if( m_colObjAttr->m_miTodStart > 0 )
    //{
    //	ypsUniversalTime todTime( (char*)&m_colObjAttr->m_miTodStart );
    //	startTime = todTime;
    //}
    // @D0D end
    // Else use the time_t field and set the long_time_t field
    else
    {
        m_colObjAttr->m_collectionStart_long = 0;
        if(m_colObjAttr->m_collectionStart != 0)
            m_colObjAttr->m_collectionStart_long = m_colObjAttr->m_collectionStart + CTIME_OFFSET;
	startTime.setTime( m_colObjAttr->m_collectionStart_long );
    }
    unlockspace( m_objBaseSpacePtr );

}


void ypsMiCollectionObject::getCollectionStartString
(  ypsString &strString )
{

    strString = ypsString( m_colObjAttr->m_startTimestamp
			   , sizeof( m_colObjAttr->m_startTimestamp ) );

}

void ypsMiCollectionObject::getCollectionEndTime
(  ypsUniversalTime &endTime )
{
    lockspace( m_objBaseSpacePtr );
    // @D0A begin  Use the long_time_t field now instead if it is set
    if( m_colObjAttr->m_collectionEnd_long > 0)
    {
	endTime.setTime (m_colObjAttr->m_collectionEnd_long);
    }
    // @D0A end
    // @D0D begin
    //else if( m_colObjAttr->m_miTodEnd > 0 )
    //{
    //	ypsUniversalTime todTime( (char*)&m_colObjAttr->m_miTodEnd );
    //	endTime = todTime;
    //}
    // @D0D end
    // Else use the time_t field and set the long_time_t field
    else
    {
        m_colObjAttr->m_collectionEnd_long = 0;
        if(m_colObjAttr->m_collectionEnd != 0)
            m_colObjAttr->m_collectionEnd_long = m_colObjAttr->m_collectionEnd + CTIME_OFFSET;
	endTime.setTime( m_colObjAttr->m_collectionEnd_long);
    }
    unlockspace( m_objBaseSpacePtr );

}

void ypsMiCollectionObject::getCollectionEndString
(  ypsString &endString )
{
    endString = ypsString( m_colObjAttr->m_endTimestamp
			   , sizeof( m_colObjAttr->m_endTimestamp ) );
}

int ypsMiCollectionObject::isExpired( )
{
    lockspace( m_objBaseSpacePtr );
    uint32 objRetHours = m_colObjAttr->m_objRetentionHours;
    unlockspace( m_objBaseSpacePtr );

    if( objRetHours == 0 )
    {
	return 0;
    }
    if( objRetHours == INT_MAX )
    {
	return 0;
    }
    ypsUniversalTime curTime;
    ypsUniversalTime endTime;
    getCollectionEndTime( endTime );

    long_time_t endTime_t = endTime.getTime( );
    long_time_t curTime_t = curTime.getTime( );

    long expireSecs = objRetHours * 60 * 60;
    long elapsedSecs = long_difftime( curTime_t, endTime_t );

    return( elapsedSecs > expireSecs );
}

void ypsMiCollectionObject::getCollectionExpireTime
( ypsUniversalTime &expireTime )
{
    lockspace( m_objBaseSpacePtr );
    uint32 objRetHours = m_colObjAttr->m_objRetentionHours;
    unlockspace( m_objBaseSpacePtr );

    if( objRetHours == 0 )
    {
	expireTime.setTime( 0 );
	return;
    }
    if( objRetHours == INT_MAX )
    {
	expireTime.setTime( -1 );
	return;
    }

    ypsUniversalTime endTime;

    getCollectionEndTime( endTime );

    long_time_t expireTime_t =  endTime.getTime( );   // @D0C
    expireTime_t += ( objRetHours * 60 * 60 );

    expireTime.setTime( expireTime_t );

}
void ypsMiCollectionObject::setCollectionExpireTime
( ypsUniversalTime &expireTime )
{
    ypsUniversalTime endTime;

    getCollectionEndTime( endTime );
    
    long_time_t curEndTime_t = endTime.getTime( );  // @D0C

    long_time_t newExpireTime_t = expireTime.getTime( );  // @D0C

    uint32 tmpHours = 0;

    if( newExpireTime_t == -1 )
    {
	tmpHours = INT_MAX;
    }
    int tmpSecs = 0;
    tmpSecs = newExpireTime_t - curEndTime_t;

    if( tmpSecs > 0 )
    {
	tmpHours = tmpSecs / 3600;
	if( tmpHours == 0 )
	    tmpHours = 1;
   }

    lockspace( m_objBaseSpacePtr );
    m_colObjAttr->m_objRetentionHours = tmpHours;
    unlockspace( m_objBaseSpacePtr );

    // now update CCA  @D1A
    char ccaFormat[10];                              // @D5A
    memset(ccaFormat, ' ', sizeof(ccaFormat));       // @D5A
    getManagementCollectionObjectCCAType(ccaFormat); // @D5C
    if ( strncmp(ccaFormat, "          ", sizeof(ccaFormat)) != 0 ) { // @D5C
    NONEPTCALL(  QPYREGC , QSYS
	      ,( (char *)ccaFormat
		 , (char *)m_objName
		 , (char*)m_objLibrary
		 )
	      , OFF, OFF );
    }
    else;  
}


Qus_OBJD0300& ypsMiCollectionObject::retrieveObjectDesc( )
{
    char objlibname[21];

    sprintf( objlibname, "%-10.10s%-10.10s"
	     , (char*)m_objName
	     , (char*)m_objLibrary
	     );

                                   //@B7C 
                                   //@B7C 
    EPTCALL( QUSROBJD
	     ,( &m_objDesc
	      , sizeof(Qus_OBJD0300)
	      , "OBJD0300"
	      , objlibname
	      , "*MGTCOL   "
	      )
	     , OFF, OFF );

    return( m_objDesc );
}

void ypsMiCollectionObject::getCollectionPathName( ypsString &pathName )
{
    char tempStr[32];

    sprintf( tempStr, "%s/%s" , (char*)m_objLibrary, (char*)m_objName );

    pathName = tempStr;

}
void ypsMiCollectionObject::getCollectionPathName( char* pathName )
{

    sprintf( pathName, "%s/%s" , (char*)m_objLibrary, (char*)m_objName );

}

void ypsMiCollectionObject::getCollectionQualName
( ypsString &objLib, ypsString &objName )
{
    objLib  = m_objLibrary;
    objName = m_objName;
}

void ypsMiCollectionObject::getCollectionQualName
( char* objLib, char* objName )
{
    strcpy( objLib , (char*) m_objLibrary );
    strcpy( objName, (char*) m_objName    );
}

void ypsMiCollectionObject::getCollectionSystemName
( ypsString &systemName )
{
    Qus_OBJD0300 objDesc;

    char objlibname[21];

    sprintf( objlibname, "%-10.10s%-10.10s"
	     , (char*)m_objName
	     , (char*)m_objLibrary
	     );

                                   //@B7C 
                                   //@B7C 
    EPTCALL(  QUSROBJD
	      ,( &objDesc
		 , sizeof(Qus_OBJD0400)
		 , "OBJD0400"
		 , objlibname
		 , "*MGTCOL   "
		 )
	      , OFF, OFF );
 
    systemName = ypsString( objDesc.System_Object_Creation
			   , sizeof(objDesc.System_Object_Creation)
			   );
} 

unsigned int ypsMiCollectionObject::getSizeInKbytes( )
{
    Qus_OBJD0400 objDesc;

    char objlibname[21];

    sprintf( objlibname, "%-10.10s%-10.10s"
	     , (char*)m_objName
	     , (char*)m_objLibrary
	     );

                                   //@B7C 
                                   //@B7C 
    EPTCALL( QUSROBJD
	     , ( &objDesc
	      , sizeof(Qus_OBJD0400)
	      , "OBJD0400"
	      , objlibname
	      , "*MGTCOL   "
		)
	     , OFF, OFF );

    uint64 totalSizeBytes = objDesc.Object_Size;

    totalSizeBytes *= objDesc.Object_Size_Multiplier;
                                   //@B1A 
     uint32 totalKbytes = totalSizeBytes / 1024;

    return totalKbytes;
}

unsigned int ypsMiCollectionObject::getSizeInMbytes( )
{
    Qus_OBJD0400 objDesc;

    char objlibname[21];

    sprintf( objlibname, "%-10.10s%-10.10s"
	     , (char*)m_objName
	     , (char*)m_objLibrary
	     );
                                   //@B7C 
                                   //@B7C 
   EPTCALL( QUSROBJD
	    , ( &objDesc
	      , sizeof(Qus_OBJD0400)
	      , "OBJD0400"
	      , objlibname
	      , "*MGTCOL   "
		)
	     , OFF, OFF );

    uint64 totalSizeBytes = objDesc.Object_Size;

    totalSizeBytes *= objDesc.Object_Size_Multiplier;

    uint32 totalMbytes = totalSizeBytes / 1000000;


    return totalMbytes;
}

ypsRepositoryAttributes& ypsMiCollectionObject::listRepositoryAttributes
( ypsRpHandle rpHandleId )
{
    ypsRpAttributes *rpAttributesPtr = NULL;
    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandleId ) )
	{
	    m_rpLocalArray[idx]->getFirstAttrElem( m_rpAttrElem );
	    
	    break;
	}
    }
    
    return m_rpAttrElem;
    
}

ypsRpPacketableCollection *
ypsMiCollectionObject::listRepositoryAttributes( )
{
    ypsString		key;
    int                 iKey = 0;
    char                cKey[4];
                                   //@B1A 
                                   //@B1A 
                                   //@B1A
    lockspace( m_objBaseSpacePtr );
    long_time_t startTime1 = m_colObjAttr->m_collectionStart_long;
    unlockspace( m_objBaseSpacePtr );

    ypsUniversalTime sTime;
    getCollectionStartTime( sTime );
    long_time_t startTime2 = sTime.getTime( );

    long long diffTime = long_difftime(startTime2, startTime1);

    ypsRepositoryAttributes *rpAttrs = NULL;

    ypsRpPacketableCollection *aCollection = new ypsRpPacketableCollection(ypsRepositoryAttributesID);

    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	m_rpLocalArray[idx]->getFirstAttrElem( m_rpAttrElem );

	sprintf( cKey, "%3.3i", iKey );

	key = ypsCCSIDConverter::convert (ypsString(cKey), HOST_CCSID, UCS2 );
	++iKey;

	rpAttrs = new ypsRepositoryAttributes( m_rpAttrElem );
                                   //@B1A 
                                   //@B1A 
                                   //@B1A 
	long_time_t tmpTime = rpAttrs->m_startTimestamp.getTime( );
	tmpTime += diffTime;
	rpAttrs->m_startTimestamp.setTime( tmpTime );

	tmpTime = rpAttrs->m_endTimestamp.getTime( );
	tmpTime += diffTime;
	rpAttrs->m_endTimestamp.setTime( tmpTime );

	aCollection->add(key, rpAttrs );

	while( m_rpLocalArray[idx]->getNextAttrElem( m_rpAttrElem ) )
	{
	    sprintf( cKey, "%3.3i", iKey );
	    key = ypsCCSIDConverter::convert (ypsString(cKey), HOST_CCSID, UCS2 );

	    ++iKey;

	    rpAttrs = new ypsRepositoryAttributes( m_rpAttrElem );
	    aCollection->add(key, rpAttrs );

	}	      
    }
    
    return aCollection;
    
}

void
ypsMiCollectionObject::setRepositoryInterval( ypsRpHandle rpHandleId , int newInterval, ypsRpBeginEndIndicator beIndicator )
{
    
    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandleId ) )
	{
	    ypsLock lock( m_objMutex );
	    m_rpLocalArray[idx]->changeInterval( newInterval, beIndicator );

	    break;
	}
    }
    _ENSOBJ( &m_objSysPtr );


}

void ypsMiCollectionObject::setCollectionDefaultInterval( int interval )
{
                                   //$A3A 
    lockspace( m_objBaseSpacePtr );
    m_colObjAttr->m_defaultInterval = interval;
                                   //$A3A 
    unlockspace( m_objBaseSpacePtr );

}

int ypsMiCollectionObject::getCollectionDefaultInterval( )
{
    int rtnInt = 0;
                                   //$A3A 
    lockspace( m_objBaseSpacePtr );
    rtnInt = m_colObjAttr->m_defaultInterval;
                                   //$A3A 
    unlockspace( m_objBaseSpacePtr );

    return rtnInt;
}

ypsCollectionAttributes&
ypsMiCollectionObject::getCollectionAttributes( )
{
    return *m_colObjAttr;
}


int ypsMiCollectionObject::doesExist( )
{
    ypsError theError;
#pragma exception_handler (ypsHandleBasicException, theError,0,_C2_MH_ESCAPE)
    YPS_TRY(theError, "*", MSGACTION_REMOVE)
    {
                                   //$A4A This will now return a 0 if it does
                                   //$A4A not exist, if any other exception
                                   //$A4A occurs return 0 as well
	return resolveObject( 0 ); 
	    
    }
# pragma disable_handler
    YPS_CATCH(theError)
    {
	return 0;
    }     

    return 1;
}

uint64 ypsMiCollectionObject::getRepositorySize( ypsRpHandle rpHandleId )
{
    uint64 rpSize = 0;
    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandleId ) )
	{
	    rpSize = m_rpLocalArray[idx]->materializeBsoEof(  );
	    break;
	}
    }
    return rpSize;
}

void ypsMiCollectionObject::sendError
( char* msgId, char *rplText, int rplLen )
{

    Qus_EC_t      tmpEerrorCode;       
    tmpEerrorCode.Bytes_Provided = 0;

    _INVPTR  qus_inv_ptr;
    qus_inv_ptr = _INVP(0);                                    

    QusSendErrorCode( &tmpEerrorCode,
		      msgId,
		      NULL,
		      rplText,
		      rplLen,
		      NULL,
		      NULL,
		      NULL,
		      US_YES,
		      &qus_inv_ptr);

}

_SYSPTR  ypsMiCollectionObject::getSysPointer( )
{
    return m_objSysPtr;
}


void ypsMiCollectionObject::createSpace( )
{
    ypsCrtSpaceTpl crtSpcTpl;
                                   // Find the ASP number of the library
                                   // the object will be placed into
   _MOBI_Template_T aspNumberTpl;

    uint32 matObjInfoOption = 3;

    memset( &aspNumberTpl, 0, sizeof(aspNumberTpl) );

    _SYSPTR *parmPtr = &m_objLibSysPtr;
    _MATOBJIF( &aspNumberTpl, parmPtr, 3 );
    
    memset( &crtSpcTpl, 0, sizeof(crtSpcTpl) );

    crtSpcTpl.space.TmpSize = sizeof(crtSpcTpl);

    memset( &crtSpcTpl, 0, sizeof(crtSpcTpl) );

    crtSpcTpl.space.TmpSize = sizeof(crtSpcTpl);

    memset( crtSpcTpl.space.Object.Name,' ',30 );
    memcpy( crtSpcTpl.space.Object.Name, (char*)m_objName, m_objName.size() );
    crtSpcTpl.space.Object.Type_Subtype = WLI_MGTCOL; //WLI_MGTCOL;
    crtSpcTpl.space.Permanent    = 1;
    crtSpcTpl.space.Var_Length   = 1;
    crtSpcTpl.space.In_Context   = 0;
//  crtSpcTpl.space.Public       = 1;
    crtSpcTpl.space.Owner        = 0;
    crtSpcTpl.space.Auto_Extend  = 1;
//    crtSpcTpl.space.No_Track     = 1;

                                   // Set the asp number to the library's
                                   // value
    crtSpcTpl.space.Recovery_Opts[2] = aspNumberTpl.u.Opt3.ASP_Number[0];
    crtSpcTpl.space.Recovery_Opts[3] = aspNumberTpl.u.Opt3.ASP_Number[1];

    crtSpcTpl.space.Size         = 4096;
    crtSpcTpl.space.InitCh       = 0x00;
                                   //@C2C 
    crtSpcTpl.space.PerfClass[0] = 0X10;
//  *(short *)crtSpcTpl.space.Public_Auth = (short)WSYMA_STARALL;
    crtSpcTpl.space.Ext_Offset   = offsetof(ypsCrtSpaceTpl, extension );
//    crtSpcTpl.space.Library      = m_objLibSysPtr;
//    crtSpcTpl.extension.User_Profile = m_userProfileSysPtr;
    memcpy(crtSpcTpl.extension.Domain_Assigned,"\x80\x00",2);

    _CRTS_Template_T *CRTSptr = &crtSpcTpl.space;
    _CRTS( &m_objSysPtr, CRTSptr );

}


void ypsMiCollectionObject::setCollectionSystemId( )
{
                                   //$A3A 
    lockspace( m_objBaseSpacePtr );
    _MMTR_Template_T machineAttrTpl;

    memset( &machineAttrTpl, 0x00, sizeof(_MMTR_Template_T) );
    machineAttrTpl.Options.Template_Size = sizeof(_MMTR_Template_T);

    matmatr( &machineAttrTpl, _MMTR_LPAR_INFO );

    strncpy( m_colObjAttr->m_logicalSSN
	     ,  machineAttrTpl.Options.Data.LparInfo.logical_ser_num
	     , sizeof( machineAttrTpl.Options.Data.LparInfo.logical_ser_num )
	     );
    m_colObjAttr->m_logicalSSN[10] = '\0';
    m_colObjAttr->m_logicalSSN[11] = '\0';
                                   //$A3A 
    unlockspace( m_objBaseSpacePtr );

}


void ypsMiCollectionObject::setCollectionSystemId( char* logicalSSN )
{
                                   //$A3A 
    lockspace( m_objBaseSpacePtr );
    strcpy( m_colObjAttr->m_logicalSSN, logicalSSN );
    m_colObjAttr->m_logicalSSN[10] = '\0';
    m_colObjAttr->m_logicalSSN[11] = '\0';
                                   //$A3A 
    unlockspace( m_objBaseSpacePtr );

}


void ypsMiCollectionObject::getCollectionSystemId( ypsString &systemId )
{
                                   //$A3A 
    lockspace( m_objBaseSpacePtr );
    systemId = m_colObjAttr->m_logicalSSN;
                                   //$A3A 
    unlockspace( m_objBaseSpacePtr );

    systemId.stripTrailing( );
}
void ypsMiCollectionObject::getCollectionSystemId( char* systemId )
{
                                   //$A3A 
    lockspace( m_objBaseSpacePtr );
    strcpy( systemId, m_colObjAttr->m_logicalSSN );
                                   //$A3A 
    unlockspace( m_objBaseSpacePtr );

}

void ypsMiCollectionObject::getRepositoryKeyRange
( ypsRpHandle rpHandle, char* firstKey, char* lastKey )
{
    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandle ) )
	{
	    m_rpLocalArray[idx]->getKeyRange( firstKey, lastKey );
	    break;
	}
    }
}

                                   //@A1A 
                                   //$A1A 
void ypsMiCollectionObject::updateChangedDateTimeDesc( )
{
    struct chgObjDscT
    {
	int numRecs;                      
	int key;                           
	int length;                        
	char data[1];

    };                              

    char objLibName[21];                  
    char objLibRtn[10];                

    ypsAPIRc apiRc;

                 
    sprintf( objLibName, "%-10.10s%-10.10s"
	     , (char*)m_objName
	     , (char*)m_objLibrary
	     );

    chgObjDscT chgObjDscTpl;

    chgObjDscTpl.numRecs = 1;
    chgObjDscTpl.key = 16;
    chgObjDscTpl.length = 1;
    chgObjDscTpl.data[0] = '1';

                                   //@B3A Added EPTCALL
    EPTCALL( QLICOBJD
	     , ( objLibRtn
		 , objLibName
		 , "*MGTCOL   "
		 , (char*)&chgObjDscTpl
		 , &apiRc
		 )
	     , OFF, OFF );

}
                                   //$A2A 
                                   //$A2A End

                                   //@C0A Start set/getRepositoryDataSource
                                   //@C0A 
void
ypsMiCollectionObject::setRepositoryDataSource
( ypsRpHandle rpHandleId , int rpDataSource )
{
    
    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandleId ) )
	{
	    m_rpLocalArray[idx]->setDataSource( rpDataSource );

	    break;
	}
    }
}
int
ypsMiCollectionObject::getRepositoryDataSource( ypsRpHandle rpHandleId  )
{
    for( int idx = 0; idx < m_rpLocalIdx; idx++ )
    {
	if( m_rpLocalArray[idx]->handleMatch( rpHandleId ) )
	{
	    return m_rpLocalArray[idx]->getDataSource(  );

	    break;
	}
    }
    return 0;
}
                                   //@C0A 
                                   //@C0A End set/getRepositoryDataSource

                                   //@B1A New methods to perform space location
                                   //@B1A locks and unlocks 
void ypsMiCollectionObject::lockspace( void *space )
{
    char *sptr = (char*)space;

    _MI_Time  theWait;

                                   //@B2A Set max timout to 10 secs
    mitime( &theWait
                                   // Hours
	    , 0
                                   // Minutes
	    , 0
                                   // Seconds
	    , 10
                                   // Hundreths
	    , 0
	    );

    if( sptr == NULL )
	return;

    ypsError theError;

#pragma exception_handler (ypsHandleBasicException, theError,0,_C2_MH_ESCAPE )
    YPS_TRY(theError, "*", MSGACTION_REMOVE)
    {
	locksl2( sptr, theWait, _LENR_LOCK );
    }
# pragma disable_handler
    YPS_CATCH(theError)
    {
	ypsPrintf::trace("qypscolobjMiCollectionObject::lockspace - FAILURE - Job failed to get the lock msgId %s\n", ypsErrorGetId(theError)); // @D3A
    }


}
void ypsMiCollectionObject::unlockspace( void *space )
{
    char *sptr = (char*)space;

    ypsError theError;

#pragma exception_handler (ypsHandleBasicException, theError,0,_C2_MH_ESCAPE )
    YPS_TRY(theError, "*", MSGACTION_REMOVE)
    {
	unlocksl( sptr, _LENR_LOCK );
    }
# pragma disable_handler
    YPS_CATCH(theError)
    {
	ypsPrintf::trace("qypscolobjMiCollectionObject::unlockspace - FAILURE - Job failed to release the lock msgId %s\n", ypsErrorGetId(theError)); // @D3A
    }


}

//@D5A Start -----------------------------------------------------------------
//
// Name(s):  setManagementCollectionObjectType
//
// Purpose: Sets the mgtcol object type in the collection attributes
//          by calling QUSROBJD and retrieving the extended object attribute
//
//@D5A End ------------------------------------------------------------------- 
void ypsMiCollectionObject::setManagementCollectionObjectType()
{
    Qus_OBJD0200  returnData;      // For the format of the Qus_OBJD0200 
                                   // structure, see file qusrobjd.cleinc.
    char objlibname[21];
    char objectAttribute[11]; //@D8C

    memset(&returnData, 0, sizeof(returnData));
    memset(objlibname, 0, sizeof(objlibname)); // @D8C
    memset(objectAttribute, 0, sizeof(objectAttribute)); //@D8C

    sprintf( objlibname, "%-10.10s%-10.10s"
	     , (char*)m_objName
	     , (char*)m_objLibrary
	     );

    EPTCALL( QUSROBJD
	     ,( &returnData
		, sizeof(Qus_OBJD0200)
		, "OBJD0200"
		, objlibname
		, "*MGTCOL   "
		)
	     , OFF, OFF );


    strncpy(objectAttribute, returnData.Extended_Obj_Attr, sizeof(returnData.Extended_Obj_Attr)); //@D8C

    setManagementCollectionObjectType(objectAttribute);

}
//@D5A End -------------------------------------------------------------------

//@D5A Start -----------------------------------------------------------------
//
// Name(s):  setManagementCollectionObjectType
//
// Purpose: Sets the mgtcol object type in the collection attributes
//          based on the string that is passed in
//
//@D5A End ------------------------------------------------------------------- 
void ypsMiCollectionObject::setManagementCollectionObjectType(char* type)
{
    ypsString objectType(type);
    objectType.stripLeadingAndTrailing();

    // Set the object type in the collection attributes
    if ( objectType == "*PFR" )
    {
	m_colObjAttr->m_mgtcolType = YpsMgtcolTypePFR;
    }
    else if ( objectType == "*PEX" )
    {
	m_colObjAttr->m_mgtcolType = YpsMgtcolTypePEX;
    }
    else if ( objectType == "*PFRDTL" )
    {
	m_colObjAttr->m_mgtcolType = YpsMgtcolTypePFRDTL;
    }
    else if ( objectType == "*PFRHST" )
    {
	m_colObjAttr->m_mgtcolType = YpsMgtcolTypePFRHST;
    }
    else { 
	m_colObjAttr->m_mgtcolType = YpsMgtcolTypeUNKNOWN;
    }

    ypsPrintf::trace("ypsMiCollectionObject::setManagementCollectionObjectType TYPE %i - %s\n", m_colObjAttr->m_mgtcolType, type);
}

//@D5A Start -----------------------------------------------------------------
//
// Name(s):  getManagementCollectionObjectType
//
// Purpose: Gets the mgtcol object type from the collection attributes 
//
//----------------------------------------------------------------------------
YpsMgtcolObjectType ypsMiCollectionObject::getManagementCollectionObjectType()
{
    YpsMgtcolObjectType objectType = (YpsMgtcolObjectType) m_colObjAttr->m_mgtcolType;

    // If object type is unknown, try and set the type
    if (objectType == YpsMgtcolTypeUNKNOWN)
    {
	setManagementCollectionObjectType();
    }

    return (YpsMgtcolObjectType) m_colObjAttr->m_mgtcolType;
}
//@D5A End -------------------------------------------------------------------

//@D5A Start -----------------------------------------------------------------
//
// Name(s):  getManagementCollectionObjectCCAType 
//
// Purpose: Converts the *MGTCOL object type to the format used by the
//          collection services CCA function
//
// Parameters:  char* ccaFormat:  Return value parm,  10 bytes of space must exist
//              to return the value of the caa type 
//
//----------------------------------------------------------------------------
void ypsMiCollectionObject::getManagementCollectionObjectCCAType(char* ccaFormat)
{
    YpsMgtcolObjectType objectType = getManagementCollectionObjectType();

    switch (objectType)
    {
	case YpsMgtcolTypePFR:
	    strncpy(ccaFormat, "*CSMGTCOL ", 10);
	    break;
	case YpsMgtcolTypePEX:
	    strncpy(ccaFormat, "*PEXMGTCOL", 10);
	    break;
	default:
	    strncpy(ccaFormat, "          ", 10);
    }
}
//@D5A End -------------------------------------------------------------------
