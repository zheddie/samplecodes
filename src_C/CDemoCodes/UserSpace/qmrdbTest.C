//============================================================================
//
// Name: qmrdbTester.C
//
// Module Name: QMRDBTESTER
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
#include "mrdbTypes.H"
#include "qmrdbLocalConfig.H"

int main (int argc, char *argv[])
{
    int rc;
    if (argc <= 1)
    {
	rc = createLocalUserSpace();
	rc = initializeLocalUserSpace();

	rc = createSharedUserSpace();
	rc = initializeSharedUserSpace();
    }
    else
    {
	rc = destroyLocalUserSpace();
	rc = destroySharedUserSpace();
    }

    return 0;
}

