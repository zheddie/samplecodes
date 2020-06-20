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
/*     5722-SS1                                                     */
/*     5761-SS1                                                     */
/*                                                                  */
/* (C)Copyright IBM Corp.  2005, 2007                               */
/*                                                                  */
/* The Source code for this program is not published  or otherwise  */
/* divested of its trade secrets,  irrespective of what has been    */
/* deposited with the U.S. Copyright Office.                        */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

/* Change Activity:                                                 */
/*                                                                  */
/* FLAG REASON   DATE    PGMR      COMMENTS                         */
/* ---- -------  ------- ----      -------------------------------- */
/* $A1= D95440   060701  JGORZINS  Allowed for script invocation    */
/* $A2= 9B1678   060811  JGORZINS  LDR_CNTRL error-check/repair     */
/* $A3= Pnnnnnn  070129  kryka     Need 64bit version for 64bit j9  */
/* $A4= Pnnnnnn  070312  kryka     add setting of PASE_MAXSHR64 for */
/*                                 64bit PASE                       */
/* $A5= 9B52779  070502  JGORZINS  LIBPATH change and added         */
/*                                   IBM_JAVA_COMMAND_LINE          */
/* $A6=          070716  blair     Repair for change in sys/stat.h  */
/* $A7= 9C09262  070807  JGORZINS  a whole lotta changes.           */
/* $A8= 9C23904  080409  JGORZINS systemCL change                   */

#include <sys/types.h>
#include <nl_types.h>
#include <locale.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <as400_types.h>
#include <as400_protos.h>
#include <nlist.h>          //@A7a
#include <os400msg.h> // for QMHSNDPM //@A2a (@A7m)
   
#ifdef DEBUG
#define DPRINTF(x) printf x
#else
#define DPRINTF(x) /* no-op */
#endif

//@A8a added entire function
//
// We need to determine which streams we want PASE to do conversions 
// for us.  In these cases, the PASE runtime is handling output via
// a pipe, file redirection or similar mechanisms and expecting ASCII,
// since jFromPASE is a PASE binary. Thus, we want our Java output 
// converted from EBCDIC so the PASE pipe (or file) gets the ASCII it 
// expects. These cases will be indicated by  a negative
// ILE descriptor number from fstatx. 
// 
// This function returns a mask containing 0 or more of the following
// flags used by systemCL(), which indicates whether we want to enable
// separate converters for these streams:
//              SYSTEMCL_FILTER_STDIN
//              SYSTEMCL_FILTER_STDOUT
//              SYSTEMCL_FILTER_STDERR
unsigned int getCvtFlags()
{
    //return 0x00;

    
    int rc;
    int fd_ile;
    unsigned int mask = 0x00;

    rc = fstatx(0,
		(struct stat*)&fd_ile,
		sizeof(fd_ile),
		STX_XPFFD_PASE);
    if(fd_ile >= 0)
    {
	mask |= SYSTEMCL_FILTER_STDIN;
    }
    rc = fstatx(1,
		(struct stat*)&fd_ile,
		sizeof(fd_ile),
		STX_XPFFD_PASE);
    if(fd_ile >= 0)
    {
	mask |= SYSTEMCL_FILTER_STDOUT;
    }
    rc = fstatx(2,
		(struct stat*)&fd_ile,
		sizeof(fd_ile),
		STX_XPFFD_PASE);
    if(fd_ile >= 0)
    {
	mask |= SYSTEMCL_FILTER_STDERR;
    }
    printf("mask=%x\n",mask);
    char * jCnvt = getenv("QIBM_JAVA_STDIO_CONVERT");
    if((jCnvt != NULL) && strcmp(jCnvt,"N") == 0){
	return 0x00;
    }
    // if we are running inside an xterm, the VM
    //will set os400.stdio.convert=N, so no more
    // conversions necessary
    char* termVar = getenv("TERM");
    if ((termVar != NULL)&&  
       (    !strcmp(termVar,"xterm") || 
            !strcmp(termVar,"vt100")|| 
	    !strcmp(termVar,"wm") ) ) 
    {   
        return 0x00;
    }   
   
    return mask;
}



int main(int argc, char *argv[]) {                   // @A3C
     
     
     setlocale(LC_ALL, "");
     
     /*
      *  Determine the base name used to invoke this program
      *  and the Java file.encoding property that corresponds
      *  to the OS/400 PASE CCSID, then malloc enough heap
      *  storage to accomodate added properties, arguments,
      *  and worst-case expansion of embedd ed quotes.  (Heap
      *  storage is implicitly freed when this program exits)
      */
     char* myname = strrchr(argv[0], '/');           // @A3C
     if (myname)
	 ++myname;
     else
	 myname = argv[0];

     /**
      * If argv[0] is jFromPase, we assume that the next argument is
      * the actual tool name (not fully qualified)
      * This is done so that we can invoke this part from a shell 
      * script and still have it handled properly.                //@A1a
      **/
     if ((strcmp(myname,"jFromPASE")==0) ||                      // @A3C
	 (strcmp(myname,"jFromPASE64")==0))                      // @A3C
     {                                                            //@A1a
         //ignore argv[0] when parsing arguments                  //@A1a
         argv++;                                                  //@A1a
         argc--;                                                  //@A1a
         myname = argv[0];                                        //@A1a
     }                                                            //@A1a
   
     size_t cmdLen = 128 + strlen(myname); // @A3C
     for (int i = 1; i < argc; ++i)
	 cmdLen += ((2 * strlen(argv[i])) + 3);

     char* cmd = (char*)malloc(cmdLen);                 // @A3C
     
     /*
      *  Assemble the CL command string, using the -J prefix to
      *  pass properties to the JVM (rather than to the utility)
      *  for every utility except "java"
      */
     char* jopt = ((strcmp(myname, "java")) ? "-J" : "");    // @A3C
     sprintf(cmd,
	     "QSYS/CALL QSYS/QJVATOOLS "
	     "('%s'",
	     myname);
     for (int i = 1; i < argc; ++i)
     {         
	 char *parm = argv[i];
	 char *quote = strchr(parm, '\'');
	 strcat(cmd, " '");
	 while (quote)
	 {
	     sprintf((cmd + strlen(cmd)),
		     "%.*s''",
		     (quote - parm),
		     parm);
	     parm = quote + 1;
	     quote = strchr(parm, '\'');
	 }

	 sprintf((cmd + strlen(cmd)), "%s'", parm);
     }
     strcat(cmd, ")");
/*     setenv("QIBM_USE_DESCRIPTOR_STDIO","I",1);
     setenv("QIBM_PASE_DESCRIPTOR_STDIO","B",1);
     setenv("QIBM_JAVA_STDIO_CONVERT","N",1);
*/
     printf("QIBM_USE_DESCRIPTOR_STDIO=%s\n",getenv("QIBM_USE_DESCRIPTOR_STDIO"));
     printf("QIBM_PASE_DESCRIPTOR_STDIO=%s\n",getenv("QIBM_PASE_DESCRIPTOR_STDIO"));
     printf("QIBM_JAVA_STDIO_CONVERT=%s\n",getenv("QIBM_JAVA_STDIO_CONVERT"));

     /*
      *  Invoke systemCL to run the CL command  (@A8 added SYSTEMCL_FILTER_xxxx bits)
      */
     int rc = systemCL(cmd, SYSTEMCL_ENVIRON | SYSTEMCL_SPAWN  | getCvtFlags()); //@A8c
     DPRINTF(("systemCL(\"%s\", 0x%x) = %i\n", cmd, systemCL_flags, rc));
     return rc;
 }
/* make commands

xlC -g -qlist -lpthreads -qsource -bI:/osxpf/v6r1m0.cuep/bld/cmvc/pase.pgm/p5.cuep/lib/as400_libc.exp -I/osxpf/v6r1m0.cuep/bld/cmvc/pase.pgm/p5.cuep/include:/osxpf/v6r1m0.cuep/bld/shadow/pase.pgm/p5.cuep/include jFromPASE.C -o jFromPASE


xlC -g -qlist -q64 -lpthreads -qsource -bI:/osxpf/v6r1m0.cuep/bld/cmvc/pase.pgm/p5.cuep/lib/as400_libc.exp -I/osxpf/v6r1m0.cuep/bld/cmvc/pase.pgm/p5.cuep/include:/osxpf/v6r1m0.cuep/bld/shadow/pase.pgm/p5.cuep/include jFromPASE.C -o jFromPASE64

*/
