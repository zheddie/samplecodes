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
/*     5722-JV1  5761-JV1                                           */
/*                                                                  */
/* (C)Copyright IBM Corp.  2006, 2009                               */
/*                                                                  */
/* The Source code for this program is not published  or otherwise  */
/* divested of its trade secrets,  irrespective of what has been    */
/* deposited with the U.S. Copyright Office.                        */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/* Change Activity:                                                 */
/* $A0=P9B26254 5P40 Jun-12-06 jhaverma New part                    */
/* $A1=?             Sep-22-06 jgorzins Qyj9SpawnProcessWithIleEnv  */
/* $A2=pB41619  5P40 Jan-31-06 jgorzins env Changes for Runtime.exec*/
/* $A3=P9C04419 v5R4 Sep-06-07 jgorzins changes for Java6 & trace   */
/* $A4=P9C09666 v5R4 Nov-02-07 jgorzins environment tweaks          */
/* $A5=P9C34038 V5R4 Nov-01-08 jhaverma Add QIBM_XJ9_TRACE ability  */
/* $A6=P9C37710 V5R4 Feb-03-08 jhaverma Add MAC & MTU support       */
/* $A7=FW515069 V5R4 Mar-11-09 jhaverma Add Qp0zSetTerminalMode     */
/* $B0=FW527750 7D20 Oct-07-10 jhaverma Move java.xpf to java.zj9   */
/* $A8=FW529260 V5R4 Sep-07-10 bjxiangy Add LoadDnsConfig ability   */
/*==================================================================*/
#pragma comment (copyright, \
"5770JV1 (c) Copyright IBM Corp. 2006, 2013. \
  All rights reserved. \
  US Government Users Restricted Rights - \
    Use, duplication or disclosure restricted \
    by GSA ADP Schedule Contract with IBM Corp. \
    Licensed Materials - Property of IBM ")

// Note: This part originally created for J9 JDK native methods which required ILE APIs to function properly.

#include <stdio.h>
#include <stdlib.h>                // getenv
#include <errno.h>
#include <iconv.h>
#include <Qp0lstdi.h>           // Qp0lGetAttr, Qp0lGetPathFromFileID
#include "mih/modasa.h"
#include <spawn.h>
#include <sys/wait.h>
#include <qp2user.h>  //@A3a
#include "Qxj9Trace.H"  // @A5A
#include <unistd.h>  // @A5A - getpid(), access()
#include "qtocnetsts.cleinc" /* Header for Netstat API's @A6A */
#include <qusec.h> /* Header for user error structure @A6A */
#include <quscrtus.h> /* Header to create userspace @A6A */
#include <qusdltus.h> /* Header to delete userspace @A6A */
#include <qusptrus.h> /* Header to get pointer to userspace @A6A */
#include <qusgen.h> /* @A6A */
#include <sys/time.h> /* gettimeofday() @A6A */
#include <qp0ztrml.cleinc> /* Qp0zSetTerminalMode @A7A */

// Function prototypes located in other service programs.
extern "C" int getJobCCSID__10QjvaJvmXifFv(); // QjvaJvmXif.H

#define QYJ9_CCSID_UNICODE 13488  // Taken from qtqcvt.cinc
#define QYJ9_CCSID_UTF16       1200    // Taken from qtqcvt.cinc
#define SUCCESS 0
#define FAILURE -1

// Internal helper functions
static int DEBUG_FLAG = (getenv("QIBM_QYJ9NTVUTL_DEBUG") != NULL) ? 1 : 0;
static void debug_printf(char *msg, int i0, char *s0) {
    if (DEBUG_FLAG) {
	if (i0 > -1)
	    printf("%s%d\n", msg, i0);
	else if (s0 != NULL)
	    printf("%s%s\n", msg, s0);
	else
	    printf("%s\n", msg);
    }
    if (Qxj9Trace::isTraceActive(Qxj9Trace::JclNtv)) {			// @A5A
	if (i0 > -1)							// @A5A
	    Qxj9Trace::print(Qxj9Trace::JclNtv, "%s%d\n", msg, i0);	// @A5A
	else if (s0 != NULL)						// @A5A
	    Qxj9Trace::print(Qxj9Trace::JclNtv, "%s%s\n", msg, s0);	// @A5A
	else								// @A5A
	    Qxj9Trace::print(Qxj9Trace::JclNtv, "%s\n", msg);		// @A5A
    }									// @A5A
}
static void debug_printf(char *msg, char *s0) { debug_printf(msg, -1, s0) ; }
static void debug_printf(char *msg, int i0)   { debug_printf(msg, i0, NULL) ; }
static void debug_printf(char *msg) { debug_printf(msg, -1, NULL) ; }

static void debug_printf_pid(int result, int pid0, int pid1) {		// @A5A
    if (result != SUCCESS) {
	if (DEBUG_FLAG)
	    printf("PID %d unable to spawn process, errno %d\n", pid0, pid1);
	if (Qxj9Trace::isTraceActive(Qxj9Trace::JclNtv))
	    Qxj9Trace::print(Qxj9Trace::JclNtv, "PID %d unable to spawn process, errno %d\n", pid0, pid1);
    } else {
	if (DEBUG_FLAG)
	    printf("PID %d successfully spawned PID %d\n", pid0, pid1);
	if (Qxj9Trace::isTraceActive(Qxj9Trace::JclNtv))
	    Qxj9Trace::print(Qxj9Trace::JclNtv, "PID %d successfully spawned PID %d\n", pid0, pid1);
    }
}

// This function is for strings that are not null terminated, but the length is known
static void debug_printf_strict_length(char *msg, char *s0, int sLength) { // @A6A
    if (sLength > 100) {
	printf("debug_printf_strict_length logic error with sLength=%s\n", sLength);
	return;
    }
    char buffer[100];
    memset(buffer, 0x00, 100);
    memcpy(buffer, s0, sLength);
    debug_printf(msg, buffer);
}

/***** Function Specifications **********************************************
 * Purpose:     Convert a char array from one ccsid to another
 * Parameters:  to:             Target area
 *              chars_out:      Input - size of target area
 *                              Output - number of chars written to target area
 *              t_ccsid:        Target CCSID
 *              from:           Source char array
 *              f_size:         Number of source bytes
 *              s_ccsid:        Source CCSID
 * Returns:      0 (Success)
 *              -1 (Failure)
 ***************************************************************************/
static int Yj9Convert(char *to,   size_t &chars_out, int t_ccsid,
		      char *from, size_t f_size,     int s_ccsid) {
    char target[32];
    char source[32];
    iconv_t 	cd;
    int         rc;

    memset(target, 0x00, 32);
    memset(source, 0x00, 32);
    if (t_ccsid == 65535) { t_ccsid = 0; debug_printf("Yj9Convert() converted t_ccsid 65535 to 0"); }
    if (s_ccsid == 65535) { s_ccsid = 0; debug_printf("Yj9Convert() converted s_ccsid 65535 to 0"); }
    sprintf(target, "IBMCCSID%0.5d",        t_ccsid);
    sprintf(source, "IBMCCSID%0.5d0000000", s_ccsid);
    cd = iconv_open(target, source);
    if ( cd.return_value == -1 ) {
	debug_printf("Yj9Convert() iconv_open failed!");
	debug_printf("target=", target);
	debug_printf("source=", source);
        return FAILURE;
    }

    size_t initial_size = chars_out;
    rc = iconv(cd, &from, &f_size, (char**)(&to), &chars_out);
    iconv_close(cd);
    if ( rc != 0 ) {
	debug_printf("Yj9Convert() iconv failed!");
	debug_printf("target=", target);
	debug_printf("source=", source);
	debug_printf("f_size=", f_size);
	debug_printf("from=", from);
        return FAILURE;
    }

    // Determine the number of characters returned
    chars_out = initial_size - chars_out;

    return SUCCESS;
}


/***** Function Specifications **********************************************
 * Name:        GetRealPath
 * Used by:     J9 canonicalize_md.c
 * Purpose:     Return the "real path" name of the value passed in.
 *              For example: (1) User passes in /QIBM/MiXeDcAsE/hi.txt
 *                           (2) File system is case insensitive
 *                           (3) File system value is /QIBM/MixedCase/hi.txt
 *                           (4) The "real path" value from (3) is returned.
 * Parameters:  original: (in)  String containing the path to be resolved
 *              resolved: (out) String containing the "real path"
 *              rsize:    (in)  Size of resolved buffer
 *              ccsid:    (in)  CCSID of both original & resolved strings
 * Returns:      0 (Success)
 *              -1 (Failure) - resolved is not set
 ***************************************************************************/
typedef struct pn_struct {
    Qlg_Path_Name_T qlg_path_name;
    char *uniPathNamePtr;
};
typedef struct at_struct {
    Qp0l_AttrTypes_List_t qp0l_attr_array;
    int attribute;
};
typedef struct bp {
    uint offset_to_next;
    uint attr_id;
    uint attr_size;
    uint reserved;
    union {
	char pc_attr_hidden;
	char pc_attr_read_only;
	Qp0lFID_t file_id;
    };
};
extern "C" int GetRealPath(char*__ptr64 original, char*__ptr64 resolved, int rsize, int _reserved) { //@A3c
    int ccsid = Qp2paseCCSID();//@A3a
    int rc;
    unsigned int chars_out = rsize*2;
    char *buffer = (char*)modasa(chars_out);
    memset(buffer,0x00,chars_out); //@A3a
 
    rc = Yj9Convert(buffer, chars_out, QYJ9_CCSID_UNICODE,
		    original, (size_t)strlen(original), ccsid);
    if (rc != 0)
	return FAILURE;

    // Set up the path name structure for the API calls
    struct pn_struct path_name;
    memset(&path_name.qlg_path_name, 0x00, sizeof(Qlg_Path_Name_T));
    path_name.qlg_path_name.CCSID = QYJ9_CCSID_UTF16;
    path_name.qlg_path_name.Path_Type = QLG_PTR_DOUBLE; 
    path_name.qlg_path_name.Path_Name_Delimiter[1] = 0x2F; // Unicode forward slash
    path_name.qlg_path_name.Path_Length = chars_out;
    path_name.uniPathNamePtr = buffer;

    // Set up parms for GetAttr call
    struct at_struct attr_array;
    attr_array.attribute = QP0L_ATTR_FILE_ID;
    struct bp attr_buffer;
    memset(&attr_buffer, 0x00, sizeof(bp));
    uint buffer_size_provided = sizeof(attr_buffer);
    uint buffer_size_needed;    
    uint num_bytes_returned;
    uint follow_symlnk = 1;
    attr_array.qp0l_attr_array.Number_Of_ReqAttrs = 1;

    // Retrieve file id attribute for the IFS file
    rc = Qp0lGetAttr(&path_name.qlg_path_name,
		     &attr_array.qp0l_attr_array,
		     (char*)&attr_buffer,
		     buffer_size_provided,
		     &buffer_size_needed,    
		     &num_bytes_returned,
		     follow_symlnk);
    if (rc < 0) {

        // for debug info ONLY!! dbuffer will have the               //@A3a
        // first 200 chars of the original filename                  //@A3a
        if (DEBUG_FLAG || Qxj9Trace::isTraceActive(Qxj9Trace::JclNtv))   // @A5C
        {
            char *dbuffer = (char*)modasa(222);                          //@A3a
            memset(dbuffer,0x00,222);                                    //@A3a
            unsigned int bignumber = 211;                                //@A3a
            rc = Yj9Convert(dbuffer, bignumber, 0,
		            original, (size_t)strlen(original), ccsid);  //@A3a
	    if (DEBUG_FLAG)						 // @A5A
		printf("Qp0lGetAttr errno %d on '%s', strlen %d\n", errno, (char*)dbuffer, strlen(original)); // @A5C
	    if (Qxj9Trace::isTraceActive(Qxj9Trace::JclNtv))		 // @A5A
		Qxj9Trace::print(Qxj9Trace::JclNtv, "Qp0lGetAttr errno %d on '%s', strlen %d\n", errno, (char*)dbuffer, strlen(original)); // @A5A
        }
	return FAILURE;
    }

    // Retrieve the path name in Job CCSID
    char *buffer2 = (char*)modasa(rsize);
    memset(buffer2, 0, rsize);
    char *rtnString = Qp0lGetPathFromFileID(buffer2, rsize, attr_buffer.file_id);
    if (rtnString == NULL) {
        if (DEBUG_FLAG || Qxj9Trace::isTraceActive(Qxj9Trace::JclNtv))   // @A5C
        {
            // for debug info ONLY!! dbuffer will have the               //@A3a
            // first 200 chars of the original filename                  //@A3a
            char *dbuffer = (char*)modasa(222);                          //@A3a
            memset(dbuffer,0x00,222);                                    //@A3a
            unsigned int bignumber = 211;                                //@A3a
            rc = Yj9Convert(dbuffer, bignumber, 0,
		            original, (size_t)strlen(original), ccsid);  //@A3a
	    if (DEBUG_FLAG)						 // @A5A
		printf("Qp0lGetPathFromFileID errno %d on '%s', strlen %d\n", errno, (char*)dbuffer, strlen(original)); // @A5C
	    if (Qxj9Trace::isTraceActive(Qxj9Trace::JclNtv))		 // @A5A
		Qxj9Trace::print(Qxj9Trace::JclNtv, "Qp0lGetPathFromFileID errno %d on '%s', strlen %d\n", errno, (char*)dbuffer, strlen(original)); // @A5A
        }
	return FAILURE;
    }

    // Retrieve the Job CCSID
    int job_ccsid = getJobCCSID__10QjvaJvmXifFv();

    // Populate the return pointer
    chars_out = rsize;
    rc = Yj9Convert(resolved, chars_out, ccsid,
		    buffer2, (size_t)strlen(buffer2), job_ccsid);
    if (rc != 0)
	return FAILURE;
    resolved[chars_out] = 0x00;

    return SUCCESS;
}


/***** Function Specifications **********************************************
 * Name:        getJobCharArray
 * Purpose:     Converts input char array to another CCSID.  The result is
 *              returned in new storage which must be freed later.
 * Parameters:  array_in:        Char array to be converted
 *              array_in_ccsid:  CCSID of array_in
 *              array_out_ccsid: CCSID of char array returned by the function
 * Returns:     Converted char array in new storage (Success)
 *              NULL (Failure)
 ***************************************************************************/
static char* getJobCharArray(char* array_in, int array_in_ccsid, int array_out_ccsid) {
    unsigned int buffer_length = (strlen(array_in)+1)*2; // +1 for Null terminator (x2 in case it is DBCS characters)
    char *buffer = new char[buffer_length];
    memset(buffer, 0x00, buffer_length);
    int rc = Yj9Convert(buffer, buffer_length, array_out_ccsid, array_in, (size_t)strlen(array_in), array_in_ccsid);
    if (rc != 0) {
	printf("getJobCharArray - Yj9Convert failure, array_in_ccsid=%d, job_ccsid=%d\n", array_in_ccsid, array_out_ccsid);
	return NULL;
    }
    return buffer;
}


/***** Function Specifications **********************************************
 * Name:        Qyj9SpawnProcess
 * Used by:     J9 UNIXProcess_md.c
 * Purpose:     Spawn a new child process from ILE.
 * Parameters:  stdin_fd:  stdin pipe for the child process
 *              stdout_fd: stdout pipe for the child process
 *              stderr_fd: stderr pipe for the child process
 *              _reserved: not used                                          //@A3c
 *              jobname:   String containing the jobname desired for the child
 *              path:      Path for chdir in the child process (May be NULL)
 *              argv_in:   Argument array for the child process, which includes the command
 * Returns:     Process ID integer (Success)
 *              Negative value (Failure)
 ***************************************************************************/
extern "C" int Qyj9SpawnProcess(int stdin_fd, int stdout_fd, int stderr_fd, int _reserved, char*__ptr64 jobname, //@A3c
				char*__ptr64 path, char*__ptr64*__ptr64 argv_in, char*__ptr64*__ptr64 envv_in) {
    int ccsid = Qp2paseCCSID();  //@A3a
    int job_ccsid = getJobCCSID__10QjvaJvmXifFv();

    // File descriptor map
    int fdMap[3];
    fdMap[0] = stdin_fd;
    fdMap[1] = stdout_fd;
    fdMap[2] = stderr_fd;

    // Inheritance structure
    inheritance_t inherit;
    memset(&inherit, 0, sizeof(inheritance_t));
    inherit.flags = SPAWN_SETTHREAD_NP | SPAWN_SETSIGDEF | SPAWN_SETJOBNAMEARGV_NP;
    inherit.pgroup = SPAWN_NEWPGROUP;
    inherit.sigdefault.sig_bitmask.sigterm = 1;
    inherit.sigdefault.sig_bitmask.sighup  = 1;
    inherit.sigdefault.sig_bitmask.sigquit = 1;
    inherit.sigdefault.sig_bitmask.sigint  = 1;

    // Build args - Create the pointer array
    int index = 0;
    while (argv_in[index] != NULL) { index++; }
    index += 3; // (1) Jobname (2) path for chdir (3) null terminator
    char **args = (char**)modasa(index*sizeof(char*));

    // Build args - Jobname
    args[0] = getJobCharArray((char*)jobname, ccsid, job_ccsid);

    // Build args - chdir path
    if (path != NULL) { args[1] = getJobCharArray((char*)path, ccsid, job_ccsid); }
    else {
	args[1] = new char[strlen("QJVA_NO_DIRECTORY")+1];
	memcpy(args[1], "QJVA_NO_DIRECTORY", strlen("QJVA_NO_DIRECTORY"));
	args[1][strlen("QJVA_NO_DIRECTORY")] = 0x00;
    }

    // Build args - Convert argv_in to job ccsid
    index = 0;
    while (argv_in[index] != NULL) {
	args[index+2] = getJobCharArray((char*)argv_in[index], ccsid, job_ccsid);
	index++;
    }
    args[index+2] = NULL;

    // Build envs - Create the pointer array & convert envv_in to job ccsid
    index = 0;
    while (envv_in[index] != NULL) { index++; }
    index++; // null terminator
    char **envs = (char**)modasa(index*sizeof(char*));
    index = 0;
    while (envv_in[index] != NULL) {
	envs[index] = getJobCharArray((char*)envv_in[index], ccsid, job_ccsid);
	index++;
    }
    envs[index] = NULL;

    if (DEBUG_FLAG) { // Print out the parameters passed to the function
	printf("\nQyj9SpawnProcess ----------------- BEGIN PARAMETER DUMP -----------------\n");
	printf("stdin_fd=%d, stdout_fd=%d, stderr_fd=%d, ccsid=%d, job_ccsid=%d\n",
	           stdin_fd,    stdout_fd,    stderr_fd,    ccsid,    job_ccsid);
	index = 0;
	while (args[index] != NULL) {
	    printf("args[%0.2d]: >%s<\n", index, args[index]);
	    index++;
	}
	index = 0;
	while (envs[index] != NULL) {
	    printf("envs[%0.2d]: >%s<\n", index, envs[index]);
	    index++;
	}
	printf("Qyj9SpawnProcess -----------------  END PARAMETER DUMP  -----------------\n"); // @A5A
    }

    // Print out the parameters passed to the function for XJ9 tracing @A5A (Block Add)
    if (Qxj9Trace::isTraceActive(Qxj9Trace::JclSpawn)) {
	Qxj9Trace::print(Qxj9Trace::JclSpawn, "Qyj9SpawnProcess ----------------- BEGIN PARAMETER DUMP -----------------\n");
	Qxj9Trace::print(Qxj9Trace::JclSpawn, "stdin_fd=%d, stdout_fd=%d, stderr_fd=%d, ccsid=%d, job_ccsid=%d\n",
			                       stdin_fd,    stdout_fd,    stderr_fd,    ccsid,    job_ccsid);
	index = 0;
	while (args[index] != NULL) {
	    Qxj9Trace::print(Qxj9Trace::JclSpawn, "args[%0.2d]: >%s<\n", index, args[index]);
	    index++;
	}
	index = 0;
	while (envs[index] != NULL) {
	    Qxj9Trace::print(Qxj9Trace::JclSpawn, "envs[%0.2d]: >%s<\n", index, envs[index]);
	    index++;
	}
	Qxj9Trace::print(Qxj9Trace::JclSpawn, "Qyj9SpawnProcess -----------------  END PARAMETER DUMP  -----------------\n");
    }

    // Spawn target_program in a new child process.
    const char *target_program = "/QSYS.LIB/QJAVA.LIB/QJVAEXEC.PGM"; // @B0C
    pid_t pid = spawnp(target_program, 3, fdMap, &inherit, args, envs);
    if (pid < 0) {
	debug_printf_pid(FAILURE, (int)getpid(), errno);// @A5C
	return -1*errno;
    }
    debug_printf_pid(SUCCESS, (int)getpid(), pid);	// @A5C

    // Free storage
    index = 0;
    while (args[index] != NULL) {
	delete[] args[index];
	index++;
    }
    index = 0;
    while (envs[index] != NULL) {
	delete[] envs[index];
	index++;
    }

    return pid;
}


/***** Function Specifications **********************************************
 * Name:        Qyj9WaitForProcessExit
 * Used by:     J9 UNIXProcess_md.c
 * Purpose:     Perform a waitpid() on an ILE pid.  PASE is not able to handle.
 * Parameters:  pid:  pid of the child process created by Qyj9SpawnProcess
 * Returns:     Exit status
 ***************************************************************************/
extern "C" int Qyj9WaitForProcessExit(int pid) {
    debug_printf("Qyj9WaitForProcessExit pid = ", pid);
    int status;
    // Wait for the child process to exit.  This returns immediately if the child has already exited.
    int rc = waitpid(pid, &status, 0);

    // Should never happen since we did not specify WNOHANG
    if (rc == 0)
	return -1;
    else if (rc < 0) { // Something went wrong
	debug_printf("Qyj9WaitForProcessExit waitpid failure, errno = ", errno);
	if (errno == ECHILD) // Someone else has waited for the process
	    return 0;
	else return -1;
    }

    // We successfully retrieved the status from our target process
    debug_printf("Qyj9WaitForProcessExit status = ", WEXITSTATUS(status));
    return WEXITSTATUS(status);
}


/***** Function Specifications **********************************************
 * Name:        Qyj9DestroyProcess
 * Used by:     J9 UNIXProcess_md.c
 * Purpose:     End the child process now.  PASE is not able to handle.
 * Parameters:  pid:  pid of the child process created by Qyj9SpawnProcess
 * Returns:     None.
 ***************************************************************************/
extern "C" void Qyj9DestroyProcess(int pid) {
    debug_printf("Qyj9DestroyProcess pid = ", pid);
    int rc = kill(pid, SIGTERM);
    if (rc != 0)
	debug_printf("Qyj9DestroyProcess kill failure, errno = ", errno);
    else debug_printf("Qyj9DestroyProcess successful");
}



/***** Function Specifications **********************************************
 * Name:        Qyj9SpawnProcessWithIleEnv
 * Used by:     J9 UNIXProcess_md.c
 * Purpose:     Spawn a new child process from ILE.
 * Parameters:  stdin_fd:  stdin pipe for the child process
 *              stdout_fd: stdout pipe for the child process
 *              stderr_fd: stderr pipe for the child process
 *              ccsid:     CCSID of the input char* parameters
 *              jobname:   String containing the jobname desired for the child
 *              path:      Path for chdir in the child process (May be NULL)
 *              argv_in:   Argument array for the child process, which includes the command
 * Returns:     Process ID integer (Success)
 *              Negative value (Failure)
 ***************************************************************************/
 
#include <qp2user.h>

// why is it leaky? you tell me....
static inline char* leaky_get_ebc(char* toConv, int ccsid)
{
   unsigned int size = 1+strlen(toConv);
   char* ret = (char*)Qp2malloc(size,NULL);
   
   Yj9Convert(ret, size, getJobCCSID__10QjvaJvmXifFv(), 
                            toConv,  size, ccsid );
   return ret;
}
//@A1a added entire function
extern "C" int Qyj9SpawnProcessWithIleEnv(int stdin_fd, int stdout_fd, int stderr_fd, int _reserved, char*__ptr64 jobname, 
				char*__ptr64 path, char*__ptr64*__ptr64 argv_in, char*__ptr64*__ptr64 envv_in) { //@A3c
    int ccsid = Qp2paseCCSID(); //@A3a

    
    // resolve to current ILE environment
    extern char** environ;    
    
                        
    
    //count number of env's
    unsigned int index = 0;    
    while (envv_in[index] != NULL) { index++; }
    for(unsigned int i=0; environ[i] != NULL; i++) { index++; }
    index++; // null terminator
  
    // allocate space for all of them in teraspace
    char*__ptr64*__ptr64 envs = (char*__ptr64*__ptr64)Qp2malloc(index*sizeof(char*), NULL);
    
    index = 0;

    // add in the env's passed to this function
    while(envv_in[index])
    {
	envs[index] = envv_in[index];
        index++;
    }
    
    // used for freeing mem
    int firstMallocIndex = index;
    
    //add in current environment
    for(unsigned int i=0; environ[i] ; i++) {

	//@A2a begin

	// We want to not include certain environment variables, because they are already handled in
	// UNIXProcess native methods (before we get here).
        bool skipThisOne = false;
        const unsigned int numExcepts = 8;  //@A4c
        const char* excepts[numExcepts] = {  "QIBM_JFROMPASE_EXEC_DONE=",      /* would break jFromPase */
                                             "QIBM_USE_DESCRIPTOR_STDIO=",     /* already handled */
                                             "QIBM_PASE_DESCRIPTOR_STDIO=",    /* already handled */
                                             "QIBM_JAVA_STDIO_CONVERT=",       /* already handled */  
                                             "QIBM_MULTI_THREADED=",           /* already handled */
                                             "IBM_JAVA_COMMAND_LINE=",         /* would cause false info //@A4a*/
                                             "LDR_CNTRL=",                     /* would cause incorrect setting for child //@A4a*/
                                             "TERMINAL_TYPE="                  /* already handled */ 
                                          };

        // skip this one if it's one of the exceptions
        for(unsigned short excIter=0; excIter<numExcepts; excIter++)
            skipThisOne |= (0 == strncmp(environ[i], excepts[excIter], strlen(excepts[excIter])));
        if(skipThisOne)
            continue;
        //@A2a end

        char*__ptr64 cur = (char*__ptr64) Qp2malloc((1+strlen(environ[i])*sizeof(char*)), NULL);  //@A2c
        unsigned int size = 1+strlen(environ[i]); 
        int rc = Yj9Convert(cur, size, ccsid,                                        //@A2c
                            environ[i],  size, getJobCCSID__10QjvaJvmXifFv() );
        
                        
        if (rc != 0) {
            // should probably do more here
	    printf("Yj9Convert failed \n");  //@A4c
        }
        envs[index] = cur; //@A2a
        index++;
    }
        
    // NULL terminator
    envs[index] = NULL;
    
    
    int returnCode = Qyj9SpawnProcess(stdin_fd, stdout_fd, stderr_fd, ccsid, jobname,
                                      path,     argv_in,   (char*__ptr64*__ptr64)envs);
    
    // free up memory that we Qp2malloc'ed
    for(unsigned int i = firstMallocIndex; envs[i]; i++)
        Qp2free(envs[i]);
    Qp2free(envs);
    
    return returnCode;
}

/***** Function Specifications **********************************************
 * Name:        setUserSpaceName
 * Used by:     QyjGetMacAddress, QyjGetMTU
 * Purpose:     Retrieve a unique Userspace name/location that does not already
 *              exist. The QtocLstPhyIfcDta API is threadsafe, however the creation
 *              of a single, generic Userspace location for the output of the API
 *              is not. If the same JVM called this 16k times in multiple threads,
 *              the output data would be consistantly overwritten and the output
 *              would either be incorrect or throw an exception. This function
 *              is intended to ensure that the Userspace does NOT exist prior to
 *              the QtocLstPhyIfcDta API call.
 * Note:        There is a separate QTEMP library for every process.
 * Parameters:  szName: (Ouput) Name of the unique Userspace location
 * Returns:     0 (Success)
 *              -1 (Failure)
 ***************************************************************************/
static int setUserSpaceName(char* szName) { // @A6A
    // Prevent an infinite loop; it should realistically never take more than 1 iteration.
    for (int i=0; i<999999; i++) {
        // Retrieve the current time as a basic random number generator
        // The microsecond field is the perfect size: 6 digits
	struct timeval now;
	if (gettimeofday(&now, NULL) < 0) {
	    debug_printf("setUserSpaceName: gettimeofday() errno=", errno);
	    return FAILURE;
	}

        // Set the name of the user space in the QTEMP library
	sprintf(szName, "QYJ9%6.6u", now.tv_usec);
	char *rd = szName + 10;
	strncpy(rd, "QTEMP     ", 10);
	szName[20]= 0x00; // Null terminate

	// See if the user space already exists
	char test[38];
	sprintf(test, "/QSYS.LIB/QTEMP.LIB/QYJ9%6.6u.USRSPC", now.tv_usec);
	test[37] = 0x00;
	if (access(test, F_OK) == 0) {
	    debug_printf("setUserSpaceName: User space already exists = ", test);
	    continue; // It already exists, try again
	}

	debug_printf("Generated user space name = ", szName);
	return SUCCESS;
    }
    return SUCCESS;
}

/***** Function Specifications **********************************************
 * Name:        Qyj9GetMacAddress
 * Used by:     J9 NetworkInterface.c
 * Purpose:     Retrieve the MAC address for a known network interface name.
 * Parameters:  ifname: (Input) Network interface name to look for
 *              mac:    (Output) Returned MAC address (if found/available)
 * Returns:     MAC address length (Success)
 *              -1 (Failure)
 ***************************************************************************/
extern "C" int QyjGetMacAddress(char*__ptr64 ifname, unsigned char*__ptr64 mac) { // @A6A
    // Convert ifname to something we can read in this location
    int ccsid = Qp2paseCCSID();
    unsigned int chars_out = 20;
    char inLD[20];
    memset(inLD, 0x00, 20);
    if ((0 != Yj9Convert(inLD, chars_out, 0,
			 ifname, (size_t)strlen(ifname), ccsid)) ||
	(chars_out == 0)) {
	debug_printf("QyjGetMacAddress: Yj9Convert failed!");
	return -1;
    }
    debug_printf("GetMacAddress begins for ", inLD);

    // Internal variables
    char szName[21];
    char szAttr[10];
    int nSize = 250000;
    char cInit = '0';
    char szAuth[10];
    char szText[50];
    char szReplace[10];
    Qus_EC_t error = { 0 };
    char *data;
    int interfaceCount, entrySize, i = -1, result = -1;

    // Initialize variables to create Userspace
    if (setUserSpaceName(szName) == FAILURE) { return -1; }
    strncpy(szAttr, "QUSCRTUS  ", 10);
    strncpy(szAuth, "*ALL      ", 10);
    memset(szText, ' ', 50);
    strncpy(szText, "QYJ9NTVUTL NetIfc Space", 23);
    strncpy(szReplace, "*YES      ", 10);

    // Create Userspace
    QUSCRTUS(szName, szAttr, nSize, &cInit, szAuth, szText, szReplace, &error);
    if (error.Bytes_Available != 0) {
	debug_printf("Create User Space failed. Exception Id: ", error.Exception_Id);
	return -1;
    } else { debug_printf("Create User Space SUCCESS"); }

    // Netstat API call
    error.Bytes_Provided = 16;
    char *cTemp = (char*)&error;
    QtocLstPhyIfcDta(szName, QTOC_IFCD0100, cTemp);
    if (error.Bytes_Available != 0) {
	debug_printf("List Physical Interface Data failed. Exception Id: ", error.Exception_Id);
    } else {
	debug_printf("List Physical Interface Data SUCCESS");

	// Get pointer to the userspace
	QUSPTRUS(szName, &data, &error);
	if (error.Bytes_Available != 0) {
	    debug_printf("Obtaining userspace pointer failed. Exception Id: ", error.Exception_Id);
	} else {
	    debug_printf("Obtaining userspace pointer SUCCESS");

	    Qus_Generic_Header_0100_t *head = (Qus_Generic_Header_0100_t *)data;
	    interfaceCount = head->Number_List_Entries;
	    debug_printf("Retrieved physical interface count: ", interfaceCount);
	    entrySize = head->Size_Each_Entry;

	    // Get pointer to the list (skip header info)
	    char *pStart = data + head->Offset_List_Data;

	    // Loop through results from netstat
	    Qtoc_LstPhyIfcDta_IFCD0100_t *pTemp;
	    for (i = 0; i < interfaceCount; i++) {
		pTemp = (Qtoc_LstPhyIfcDta_IFCD0100_t*) (pStart + (head->Size_Each_Entry * i));

	        // Is this the entry we're looking for?
		debug_printf_strict_length("Checking line description: ", pTemp->Line_Description, 10);
		if (memcmp(inLD, pTemp->Line_Description, strlen(inLD)) == 0) {
		    debug_printf_strict_length("Found MAC address: ", pTemp->Physical_Address, 17);

		    // Is this a valid MAC address?  All zeroes doesn't count!
		    if (memcmp(pTemp->Physical_Address, "00:00:00:00:00:00", 17) == 0) {
			debug_printf("MAC address was not returned because it was all zeroes.");
		    }
		    // Is the MAC address active?
		    else if (pTemp->Physical_Interface_Status != QTOC_PI_ACTIVE) {
			debug_printf("Unable to return MAC address; it is not active. Status = ",
				     pTemp->Physical_Interface_Status);
		    } else {
			// i5/OS MAC address is always returned in XX:XX:XX:XX:XX:XX format
			int number;
			for (i = 0; i < 6; i++) {
			    sscanf(&(pTemp->Physical_Address)[i*3], "%02x", &number);
			    mac[i] = (unsigned char) number;
			}
			result = 6;
		    }
		    break; // Exit for loop
		}
	    }
	}
    }

    // Delete Userspace
    QUSDLTUS(szName, &error);
    if (error.Bytes_Available != 0)
	debug_printf("Delete User Space failed. Exception Id: ", error.Exception_Id);
    else { debug_printf("Delete User Space SUCCESS"); }

    if (result == -1)
	debug_printf("No active line description was found for ", inLD);

    return result;
}

/***** Function Specifications **********************************************
 * Name:        Qyj9GetMTU
 * Used by:     J9 NetworkInterface.c
 * Purpose:     Retrieve the Maximum Transmission Unit (MTU) for a known network interface name.
 * Parameters:  ifname: (Input) Network interface name to look for
 * Returns:     MTU (Success)
 *              -1 (Failure)
 ***************************************************************************/
extern "C" int QyjGetMTU(char*__ptr64 ifname) { // @A6A
    // Convert ifname to something we can read in this location
    int ccsid = Qp2paseCCSID();
    unsigned int chars_out = 20;
    char inLD[20];
    memset(inLD, 0x00, 20);
    if ((0 != Yj9Convert(inLD, chars_out, 0,
			 ifname, (size_t)strlen(ifname), ccsid)) ||
	(chars_out == 0)) {
	debug_printf("QyjGetMTU: Yj9Convert failed!");
	return -1;
    }
    debug_printf("QyjGetMTU begins for ", inLD);

    // Internal variables
    char szName[21];
    char szAttr[10];
    int nSize = 250000;
    char cInit = '0';
    char szAuth[10];
    char szText[50];
    char szReplace[10];
    Qus_EC_t error = { 0 };
    char *data;
    int interfaceCount, entrySize, i = -1, result = -1;

    // Initialize variables to create Userspace
    if (setUserSpaceName(szName) == FAILURE) { return -1; } 
    strncpy(szAttr, "QUSCRTUS  ", 10);
    strncpy(szAuth, "*ALL      ", 10);
    memset(szText, ' ', 50);
    strncpy(szText, "QYJ9NTVUTL NetIfc Space", 23);
    strncpy(szReplace, "*YES      ", 10);

    // Create Userspace
    QUSCRTUS(szName, szAttr, nSize, &cInit, szAuth, szText, szReplace, &error);
    if (error.Bytes_Available != 0) {
	debug_printf("Create User Space failed. Exception Id: ", error.Exception_Id);
	return -1;
    } else { debug_printf("Create User Space SUCCESS"); }

    // Netstat API call
    error.Bytes_Provided = 16;
    char *cTemp = (char*)&error;
    QtocLstNetIfc(szName, QTOC_NIFC0200, cTemp);
    if (error.Bytes_Available != 0) {
	debug_printf("List Physical Interface Data failed. Exception Id: ", error.Exception_Id);
    } else {
	debug_printf("List Physical Interface Data SUCCESS");

	// Get pointer to the userspace
	QUSPTRUS(szName, &data, &error);
	if (error.Bytes_Available != 0) {
	    debug_printf("Obtaining userspace pointer failed. Exception Id: ", error.Exception_Id);
	} else {
	    debug_printf("Obtaining userspace pointer SUCCESS");

	    Qus_Generic_Header_0100_t *head = (Qus_Generic_Header_0100_t *)data;
	    interfaceCount = head->Number_List_Entries;
	    debug_printf("Retrieved physical interface count: ", interfaceCount);
	    entrySize = head->Size_Each_Entry;

	    // Get pointer to the list (skip header info)
	    char *pStart = data + head->Offset_List_Data;

	    // Loop through results from netstat
	    Qtoc_LstNetIfc_NIFC0200_t *pTemp;
	    for (i = 0; i < interfaceCount; i++) {
		pTemp = (Qtoc_LstNetIfc_NIFC0200_t*) (pStart + (head->Size_Each_Entry * i));

	        // Is this the entry we're looking for?
		debug_printf_strict_length("Checking line description: ", pTemp->Line_Name, 10);
		if (memcmp(inLD, pTemp->Line_Name, strlen(inLD)) == 0) {
		    result = (int)pTemp->MTU_Current;
		    debug_printf("Found MTU: ", result);
		    break; // Exit for loop
		}
	    }
	}
    }

    // Delete Userspace
    QUSDLTUS(szName, &error);
    if (error.Bytes_Available != 0)
	debug_printf("Delete User Space failed. Exception Id: ", error.Exception_Id);
    else { debug_printf("Delete User Space SUCCESS"); }

    if (result == -1)
	debug_printf("No MTU was found for ", inLD);

    return result;
}


/***** Function Specifications **********************************************
 * Name:        Qyj9SetEcho @A7A
 * Used by:     Console_md.c
 * Purpose:     PASE is not able to handle when not using xterm; go ILE route instead
 * Parameters:  on:  flag to turn echo on (TRUE) or off (FALSE)
 * Returns:     Exit status (0 on success; errno on failure)
 ***************************************************************************/
extern "C" int Qyj9SetEcho(int on) {
    debug_printf("Qyj9SetEcho on = ", on);
    int result = 0;
    if (on)
	result = Qp0zSetTerminalMode(QP0Z_TERMINAL_INPUT_MODE, QP0Z_TERMINAL_NORMAL, NULL);
    else
	result = Qp0zSetTerminalMode(QP0Z_TERMINAL_INPUT_MODE, QP0Z_TERMINAL_HIDDEN, NULL);

    debug_printf("Qyj9SetEcho result = ", result);
    return result;
}


/***** Function Specifications **********************************************
 * Name:        LoadDnsConfig - @A8
 * Used by:     J9 ResolverConfigurationImpl.c
 * Purpose:     Retrieve the provisioned DNS informaiton on the iSeries. 
 * Parameters:  domainNameServers:  Store the dns server addresses, seperated by space
                searchList: Store the dns search list
                _length: Buffer length
                _reserved: for future use
 * Returns:     FAILURE
 *              SUCCESS
 ***************************************************************************/
 extern "C" int LoadDnsConfig ( char*__ptr64  domainNameServers, 
                                char*__ptr64  searchList, 
                                int _length, int _reserved) { //@A3c
 
    // Struct for retrieving the full QtocRtvTCPA TCPA1400 format
    typedef struct Combine {
        Qtoc_RtvTCPA_TCPA1100_t ignore;
        Qtoc_RtvTCPA_TCPA1400_t my1400;
        Qtoc_RtvTCPA_Inet_Addr_IP4_IP6_t myAddress1;
        Qtoc_RtvTCPA_Inet_Addr_IP4_IP6_t myAddress2;
        Qtoc_RtvTCPA_Inet_Addr_IP4_IP6_t myAddress3;
    } Combine_t;
    
    // Parameters
    Combine_t output;
    unsigned long nArrayLength = sizeof(output);
    char szFormatName[8];
    memcpy(szFormatName, "TCPA1400", 8);
    Qus_EC_t Error_Code = { 0 };
    
    QtocRtvTCPA((char *)&output,
                &nArrayLength,
                szFormatName,
                (char*)&Error_Code);

    // If QtocRtvTCPA failed, do not go any farther
    if (Error_Code.Bytes_Available != 0) {
        debug_printf("Can't get any dns server address!");
	      return FAILURE;
    } 

    // Retrieve the Job CCSID
    int ccsid = Qp2paseCCSID();
    int job_ccsid = getJobCCSID__10QjvaJvmXifFv();
    int rc;
    unsigned int rsize = _length;
    
    char *buffer = (char*)modasa(_length);
    memset(buffer,0x00,_length);     
        
    // Retrieve the nameservers jstring value, separated by spaces (3 max)
    if (output.my1400.Number_Of_DNS_Addresses > 0) { 
        strcpy(buffer, output.myAddress1.DNS_Internet_Address);
    
        if (output.my1400.Number_Of_DNS_Addresses > 1) {
            strcat(buffer, " ");
            strcat(buffer, output.myAddress2.DNS_Internet_Address);
        }
        if (output.my1400.Number_Of_DNS_Addresses > 2) {
            strcat(buffer, " ");
            strcat(buffer, output.myAddress3.DNS_Internet_Address);
        }
        buffer[_length-1]=0x00;
        debug_printf("Dns server address buffer: ", buffer);
        rc = Yj9Convert(domainNameServers, rsize, ccsid,
		            buffer, (size_t)strlen(buffer), job_ccsid);
        if (rc != 0)
	           return FAILURE;
	      domainNameServers[rsize-1] = 0x00;
    
   }

    char *buffer2 = (char*)modasa(_length);
    memset(buffer2,0x00,_length); 
     
    // If *DFT, return NULL and use generic domain name
    if (strstr(output.my1400.Domain_Search_List, "*DFT") == NULL) {
       if(output.my1400.Domain_Search_List[0]) 
    	     strncpy(buffer2,output.my1400.Domain_Search_List,rsize);
    	 buffer2[_length-1]=0x00;
    	 debug_printf("Search list buffer: ", buffer2);
       rc = Yj9Convert(searchList, rsize, ccsid,
		            buffer2, (size_t)strlen(buffer2), job_ccsid);
       if (rc != 0)
	         return FAILURE;
	     searchList[rsize-1] = 0x00;
    }
    else {
        debug_printf("*DFT - Use generic domain name");
    }
    
   return SUCCESS;
}
