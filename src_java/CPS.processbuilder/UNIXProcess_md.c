/*========================================================================
 * IBM Confidential
 * OCO Source Materials
 *
 * IBM SDK, Java(tm) Technology Edition, v8
 * (C) Copyright IBM Corp. 1995, 2016. All Rights Reserved
 *
 * The source code for this program is not published or otherwise divested of
 * its trade secrets, irrespective of what has been deposited with the U.S.
 * Copyright office.
 *========================================================================
 */
/*
 * Copyright (c) 1995, 2013, Oracle and/or its affiliates. All rights reserved.
 * ORACLE PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

/*                                                                              //IBM-zos_bringup
 * For z/OS it is simplest if we build this file EBCDIC and don't use the atoe  //IBM-zos_bringup
 * wrapper functions, since Java_java_lang_UNIXProcess_forkAndExec is passes    //IBM-zos_bringup
 * byte arrays of EBCDIC text (which will be in the correct encoding for the    //IBM-zos_bringup
 * system spawn function.                                                       //IBM-zos_bringup
 */                                                                             //IBM-zos_bringup
#ifdef __MVS__                                                                  //IBM-zos_bringup
#undef IBM_ATOE                                                                 //IBM-zos_bringup
#pragma convlit(suspend)                                                        //IBM-zos_bringup
#include <spawn.h>                                                              //IBM-zos_bringup
#include <atoe.h>                                                               //IBM-zos_bringup
#define TOASCII(s) e2a_string(s)                                                //IBM-zos_bringup
#else                                                                           //IBM-zos_bringup
#define TOASCII(s) s                                                            //IBM-zos_bringup
#endif                                                                          //IBM-zos_bringup
                                                                                //IBM-zos_bringup
                                                                                //IBM-zos_bringup
#undef  _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE 1

#include "jni.h"
#include "jvm.h"
#include "jvm_md.h"
#include "jni_util.h"
#include "io_util.h"

/*
 * Platform-specific support for java.lang.Process
 */
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#if defined(__solaris__) || defined(_ALLBSD_SOURCE) || defined(_AIX)
#include <spawn.h>
#endif

#include "childproc.h"

/*ibm@JSE-8409 start*/ /*IBM-ios_bringup*/                                      //IBM-iseries_bringup
#if defined(AIX)                                                                //IBM-iseries_bringup
#include "as400_types.h"                                                        //IBM-iseries_bringup
#include "as400_protos.h"                                                       //IBM-iseries_bringup
#include "Xj9Interfaces.hpp"                                                    //IBM-iseries_bringup

                                                                                //IBM-iseries_bringup
#define ROUND_QUAD(x) (((size_t)(x) + 0xf) & ~0xf)                              //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
static int DEBUG_FLAG = 0;                                                      //IBM-iseries_bringup
static void debug_printf(char *msg) {                                           //IBM-iseries_bringup
    if (DEBUG_FLAG) { printf("%s\n", msg); }                                    //IBM-iseries_bringup
}                                                                               //IBM-iseries_bringup
static void debug_printf_char(char *msg, char *s0)                              //IBM-iseries_bringup
{ if (DEBUG_FLAG)                                                               //IBM-iseries_bringup
    { printf("%s%s\n", msg, s0); }                                              //IBM-iseries_bringup
}                                                                               //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
/********************************************************************           //IBM-iseries_bringup
 * FUNCTION NAME: Spawn_In_ILE                                                  //IBM-iseries_bringup
 *                                                                              //IBM-iseries_bringup
 * DESCRIPTION: Support for the os400.runtime.exec property &                   //IBM-iseries_bringup
 * envvar QIBM_JAVA_RUNTIME_EXEC. Only the value 'KSH' does anything,           //IBM-iseries_bringup
 * and is not recommended.                                                      //IBM-iseries_bringup
 *                                                                              //IBM-iseries_bringup
*********************************************************************/          //IBM-iseries_bringup
static int Spawn_In_ILE(JNIEnv *env)                                            //IBM-iseries_bringup
{                                                                               //IBM-iseries_bringup
    char uppercase_ptr[6];                                                      //IBM-iseries_bringup
    memset(uppercase_ptr, 0x00, 6);                                             //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* Mimicing Classic JVM: The property wins over the envvar. */              //IBM-iseries_bringup
    jclass jls = (*env)->FindClass(env, "java/lang/System");                    //IBM-iseries_bringup
    if (jls == NULL) {                                                          //IBM-iseries_bringup
        debug_printf("Spawn_In_ILE Error: FindClass java/lang/System failed\n"); //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    else {                                                                      //IBM-iseries_bringup
        jmethodID gp = (*env)->GetStaticMethodID(env,                           //IBM-iseries_bringup
             jls, "getProperty", "(Ljava/lang/String;)Ljava/lang/String;");     //IBM-iseries_bringup
        if (gp == NULL) {                                                       //IBM-iseries_bringup
           debug_printf("Spawn_In_ILE Error: GetStaticMethodID getProperty failed\n"); //IBM-iseries_bringup
        }                                                                       //IBM-iseries_bringup
        else {                                                                  //IBM-iseries_bringup
           jstring ore = (*env)->NewStringUTF(env, "os400.runtime.exec");       //IBM-iseries_bringup
           jstring ore_value = (*env)->CallStaticObjectMethod(env, jls, gp, ore); //IBM-iseries_bringup
           (*env)->DeleteLocalRef(env, ore);                                    //IBM-iseries_bringup
           if (ore_value != NULL) {                                             //IBM-iseries_bringup
                /* No need to convert.  J9 JVM and this native method           //IBM-iseries_bringup
                running in the same job ccsid. */                               //IBM-iseries_bringup
                const char *result = (*env)->GetStringUTFChars(env, ore_value, NULL); //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
                /* Uppercase the result */                                      //IBM-iseries_bringup
                int bounds = (strlen(result) < 6) ? strlen(result) : 6;         //IBM-iseries_bringup
                for (int aa=0; aa<bounds; aa++) {                               //IBM-iseries_bringup
                    uppercase_ptr[aa] = toupper(result[aa]);                    //IBM-iseries_bringup
                }                                                               //IBM-iseries_bringup
                (*env)->ReleaseStringUTFChars(env, ore_value, result);          //IBM-iseries_bringup
            }                                                                   //IBM-iseries_bringup
        }                                                                       //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* os400.runtime.exec was not found, so try the envvar. */                  //IBM-iseries_bringup
    if (uppercase_ptr[0] == 0x00) {                                             //IBM-iseries_bringup
        const char *result = getenv("QIBM_JAVA_RUNTIME_EXEC");                  //IBM-iseries_bringup
       if (result) { /* Uppercase the result */                                 //IBM-iseries_bringup
           int bounds = (strlen(result) < 6) ? strlen(result) : 6;              //IBM-iseries_bringup
           for (int aa=0; aa<bounds; aa++)                                      //IBM-iseries_bringup
           uppercase_ptr[aa] = toupper(result[aa]);                             //IBM-iseries_bringup
       }                                                                        //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* KSH was found, so force the default J9 path. */                          //IBM-iseries_bringup
    if (memcmp(uppercase_ptr, "KSH", 3) == 0)                                   //IBM-iseries_bringup
        return 0;                                                               //IBM-iseries_bringup
    else return 1;                                                              //IBM-iseries_bringup
}                                                                               //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
/*********************************************************************          //IBM-iseries_bringup
* Function: GetFromEnvs                                                         //IBM-iseries_bringup
* Description: Searches the given env array for the named environment var       //IBM-iseries_bringup
*              starting from the end of the array. Later entries of the same    //IBM-iseries_bringup
*              envvar are allowed to overwrite prior entries. Note that         //IBM-iseries_bringup
*              the search logic only works for envvars which are of the         //IBM-iseries_bringup
*              form "XXX=yyy". That is, the '=' is required.                    //IBM-iseries_bringup
* Parameters:                                                                   //IBM-iseries_bringup
*    envs - input array of char*, each char* points to a null-terminated        //IBM-iseries_bringup
*           string containing an environment variable "XXX=yyy".                //IBM-iseries_bringup
*    envCount - the number of entries in the envs array                         //IBM-iseries_bringup
*    envName - input null-terminated string                                     //IBM-iseries_bringup
* Returns:                                                                      //IBM-iseries_bringup
*    the address of the environment variable string if successful, otherwise    //IBM-iseries_bringup
*    NULL                                                                       //IBM-iseries_bringup
*                                                                               //IBM-iseries_bringup
* See notes on 'Various Code Page Considerations' below.                        //IBM-iseries_bringup
*********************************************************************/          //IBM-iseries_bringup
static char* GetFromEnvs(char** env, int envCount, const char* envName)         //IBM-iseries_bringup
{                                                                               //IBM-iseries_bringup
    // set index 'i' to the last entry and search backwards -- note that        //IBM-iseries_bringup
    // 'i' getting the -1 is OK here, since we will just skip the loop          //IBM-iseries_bringup
    int i = envCount - 1;                                                       //IBM-iseries_bringup
    int envNameLen = strlen(envName);                                           //IBM-iseries_bringup
    while ( i >= 0 ) {                                                          //IBM-iseries_bringup
   // if the current envvar starts with the envName                        //IBM-iseries_bringup
   if ( !strncmp(env[i],envName,envNameLen) ) {                            //IBM-iseries_bringup
       // and the next char is '=', then we have a match, return the       //IBM-iseries_bringup
       // address of the string                                            //IBM-iseries_bringup
       if ( env[i][envNameLen] == '=' ) {                                  //IBM-iseries_bringup
       return env[i];                                                  //IBM-iseries_bringup
       }                                                                   //IBM-iseries_bringup
   }                                                                       //IBM-iseries_bringup
   i--;                                                                    //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    // did not find the named environment var anywhere in envs, return NULL     //IBM-iseries_bringup
    return NULL;                                                                //IBM-iseries_bringup
}                                                                               //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
/*********************************************************************          //IBM-iseries_bringup
*Function: Get_Job_Name                                                         //IBM-iseries_bringup
*Description: Support for the os400.runtime.exec.jobname property.              //IBM-iseries_bringup
*********************************************************************/          //IBM-iseries_bringup
static char * Get_Job_Name(JNIEnv *env, const char *pprog)                      //IBM-iseries_bringup
{                                                                               //IBM-iseries_bringup
    char worker[11];                                                            //IBM-iseries_bringup
    memset(worker, 0x00, 11);                                                   //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    jclass jls = (*env)->FindClass(env, "java/lang/System");                    //IBM-iseries_bringup
    if (jls == NULL) {                                                          //IBM-iseries_bringup
         debug_printf("Get_Job_Name Error: FindClass java/lang/System failed\n"); //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    else {                                                                      //IBM-iseries_bringup
        jmethodID gp = (*env)->GetStaticMethodID(env, jls,                      //IBM-iseries_bringup
              "getProperty", "(Ljava/lang/String;)Ljava/lang/String;");         //IBM-iseries_bringup
         if (gp == NULL) {                                                 //IBM-iseries_bringup
            debug_printf("Get_Job_Name Error: \
                         GetStaticMethodID getProperty failed\n");         //IBM-iseries_bringup
         }                                                                 //IBM-iseries_bringup
        else {                                                                  //IBM-iseries_bringup
             jstring orej =                                                //IBM-iseries_bringup
                    (*env)->NewStringUTF(env, "os400.runtime.exec.jobname"); //IBM-iseries_bringup
             jstring orej_value =                                          //IBM-iseries_bringup
                    (*env)->CallStaticObjectMethod(env, jls, gp, orej);    //IBM-iseries_bringup
             (*env)->DeleteLocalRef(env, orej);                            //IBM-iseries_bringup
             if (orej_value != NULL) {                                     //IBM-iseries_bringup
               /* No need to convert.  J9 JVM and this native */                //IBM-iseries_bringup
               /* method running in the same job ccsid.       */                //IBM-iseries_bringup
                   const char *result =                                //IBM-iseries_bringup
                        (*env)->GetStringUTFChars(env, orej_value, NULL); //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
               /* Check for special value of *GEN. If specified, */             //IBM-iseries_bringup
               /* generate the jobname from the command.         */             //IBM-iseries_bringup
                   char uppercase_ptr[4];                              //IBM-iseries_bringup
                   if (strlen(result) == 4) {                          //IBM-iseries_bringup
                       for (int aa=0; aa<4; aa++)                      //IBM-iseries_bringup
                         uppercase_ptr[aa] = toupper(result[aa]);  //IBM-iseries_bringup
                   }                                                   //IBM-iseries_bringup
                   int cmd_length = 0;                                 //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
                   if (memcmp(uppercase_ptr, "*GEN", 4) == 0) {        //IBM-iseries_bringup
                       int i;                                          //IBM-iseries_bringup
                       cmd_length = strlen(pprog);                     //IBM-iseries_bringup
                       for (i=cmd_length; i>=0; i--) {                 //IBM-iseries_bringup
                             if (pprog[i] == '/') {                //IBM-iseries_bringup
                                break;                             //IBM-iseries_bringup
                             }                                     //IBM-iseries_bringup
                       }                                               //IBM-iseries_bringup
                       int cmd_start = i + 1;                          //IBM-iseries_bringup
                       cmd_length = cmd_length - cmd_start;            //IBM-iseries_bringup
                       if (cmd_length > 10) {                          //IBM-iseries_bringup
                             cmd_length = 10;                      //IBM-iseries_bringup
                       }                                               //IBM-iseries_bringup
                       memcpy(worker, &(pprog[cmd_start]), cmd_length); //IBM-iseries_bringup
                   }                                                   //IBM-iseries_bringup
                   else {                                              //IBM-iseries_bringup
                       cmd_length = strlen(result);                    //IBM-iseries_bringup
                       if (cmd_length > 10) {                          //IBM-iseries_bringup
                             cmd_length = 10;                      //IBM-iseries_bringup
                       }                                               //IBM-iseries_bringup
                       memcpy(worker, result, cmd_length);             //IBM-iseries_bringup
                   }                                                   //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
            /* Force generated name to all upper case and replace invalid chars. */ //IBM-iseries_bringup
                   for (int i=0; i<cmd_length; i++) {                  //IBM-iseries_bringup
                       worker[i] = toupper(worker[i]);                 //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
            /* Digit is not valid in first position  (block of code moved up)*/ //IBM-iseries_bringup
                       if ((i == 0) && (worker[i] >= '0') && (worker[i] <= '9')){ //IBM-iseries_bringup
                              memset(worker, 0x00, 11); /* Use default job name */ //IBM-iseries_bringup
                              break;                               //IBM-iseries_bringup
                       }                                               //IBM-iseries_bringup
                       /* Check for a valid character */               //IBM-iseries_bringup
                       if ((worker[i] >= 'A' && worker[i] <= 'I') ||   //IBM-iseries_bringup
                             (worker[i] >= 'J' && worker[i] <= 'R') || //IBM-iseries_bringup
                             (worker[i] >= 'S' && worker[i] <= 'Z') || //IBM-iseries_bringup
                             (worker[i] >= '0' && worker[i] <= '9') || //added line //IBM-iseries_bringup
                             (worker[i] == '#')||(worker[i] == '$') || //IBM-iseries_bringup
                             (worker[i] == '@')||(worker[i] == '_')) { //IBM-iseries_bringup
                             continue;                             //IBM-iseries_bringup
                       }                                               //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
                       /* Invalid character, replace with underscore. */ //IBM-iseries_bringup
                       worker[i] = '_';                                //IBM-iseries_bringup
                   }                                                   //IBM-iseries_bringup
                   (*env)->ReleaseStringUTFChars(env, orej_value, result); //IBM-iseries_bringup
             }                                                             //IBM-iseries_bringup
         }                                                                 //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* The user supplied a valid os400.runtime.exec.jobname value. */           //IBM-iseries_bringup
    char * jobname = NULL;                                                      //IBM-iseries_bringup
    if ((worker[0] != 0x00) && (worker[0] != '_')) {                            //IBM-iseries_bringup
          jobname = malloc(strlen(worker)+1);                              //IBM-iseries_bringup
          memcpy(jobname, worker, strlen(worker));                         //IBM-iseries_bringup
          jobname[strlen(worker)] = 0x00;                                  //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    /* Default to QJVAEXEC. */                                                  //IBM-iseries_bringup
    else {                                                                      //IBM-iseries_bringup
         jobname = malloc(strlen("QJVAEXEC")+1);                           //IBM-iseries_bringup
         memcpy(jobname, "QJVAEXEC", strlen("QJVAEXEC"));                  //IBM-iseries_bringup
         jobname[strlen("QJVAEXEC")] = 0x00;                               //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    debug_printf_char("Input  Jobname=", worker);                               //IBM-iseries_bringup
    debug_printf_char("Actual Jobname=", jobname);                              //IBM-iseries_bringup
    return jobname;                                                             //IBM-iseries_bringup
}                                                                               //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
static void printArray(const char *name, const char **list)                     //IBM-iseries_bringup
{                                                                               //IBM-iseries_bringup
    if (!DEBUG_FLAG) return;                                                    //IBM-iseries_bringup
    int list_size = 0;                                                          //IBM-iseries_bringup
    printf("* * * * * * * Printing out %s array * * * * * *\n", name);          //IBM-iseries_bringup
    while (list[list_size] != NULL) {                                           //IBM-iseries_bringup
        printf("[%d]: %s\n", list_size, list[list_size]);                  //IBM-iseries_bringup
        list_size++;                                                       //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    printf("Array size = %d\n", list_size);                                     //IBM-iseries_bringup
    printf("* * * * * * * End of %s list * * * * * *\n", name);                 //IBM-iseries_bringup
}                                                                               //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
static void Throw_InternalError(JNIEnv *env, int errno_in,                      //IBM-iseries_bringup
                                int extra, const char *msg)                     //IBM-iseries_bringup
{                                                                               //IBM-iseries_bringup
    char errmsg[150];                                                           //IBM-iseries_bringup
    memset(errmsg, 0x00, 150);                                                  //IBM-iseries_bringup
    sprintf(errmsg, "%s, errno=%d, extra=%d", msg, errno_in, extra);            //IBM-iseries_bringup
    JNU_ThrowInternalError(env, errmsg);                                        //IBM-iseries_bringup
}                                                                               //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
static int getIleFD(JNIEnv *env, int pase_fd)                                   //IBM-iseries_bringup
{                                                                               //IBM-iseries_bringup
    int ile_fd = -1;                                                            //IBM-iseries_bringup
    int rc =                                                                    //IBM-iseries_bringup
          fstatx(pase_fd, (struct stat*)&ile_fd, sizeof(int), STX_XPFFD_PASE);  //IBM-iseries_bringup
    if (rc || (ile_fd < 0)) {                                                   //IBM-iseries_bringup
        Throw_InternalError(env, errno, pase_fd, "fstatx failure");        //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    return ile_fd;                                                              //IBM-iseries_bringup
}                                                                               //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
/********************************************************************           //IBM-iseries_bringup
 * FUNCTION NAME: getMallocText                                                 //IBM-iseries_bringup
 *                                                                              //IBM-iseries_bringup
 * DESCRIPTION: Return input message(s) in their own malloc'd storage.          //IBM-iseries_bringup
 * If 2 input messages, concatenate them. msg1 is always a const char           //IBM-iseries_bringup
 * with a null terminator so strlen() works. msg2 needs an input length         //IBM-iseries_bringup
 *                                                                              //IBM-iseries_bringup
*********************************************************************/          //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
static char* getMallocText(const char* msg1, char* msg2, int msg2Len) {         //IBM-iseries_bringup
    int malloc_length = strlen(msg1) + 1 + msg2Len;                             //IBM-iseries_bringup
    char* result = (char*) malloc(malloc_length);                               //IBM-iseries_bringup
    memset(result, 0x00, malloc_length);                                        //IBM-iseries_bringup
    strcat(result, msg1);                                                       //IBM-iseries_bringup
    if (msg2 != NULL)                                                           //IBM-iseries_bringup
  strncat(result, msg2, msg2Len);                                               //IBM-iseries_bringup
    return result;                                                              //IBM-iseries_bringup
}                                                                               //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup

/********************************************************************
 * FUNCTION NAME: checkEnvAndReplace
 *
 * DESCRIPTION: check and update some environment variables value.
 * If let child process output to STDIO, then need to convert these
 * output to JOB CCSID.
 * For the usage of QIBM_PASE_DESCRIPTOR_STDIO: please refer info center.
 *
*********************************************************************/
static void checkEnvAndReplace(char **envv, int inherit) {
	int javaChildExist = 0;
    int envIndex = 0;
    int paseEnv[2] = {0};
    int paseNum = 0;
    int paseExistCopy = 0;
    char *pFirstPaseMatch = NULL;

    while(envv[envIndex] != NULL) {

    	if ( ( strcmp(envv[envIndex], "QIBM_JAVA_CHILD_STDIO_CONVERT=Y")==0 ) ||
             ( strcmp(envv[envIndex], "QIBM_JAVA_CHILD_STDIO_CONVERT=N")==0 ) ) {
    	    javaChildExist = 1;
    	}

        if ( (strcmp(envv[envIndex], "QIBM_JAVA_CHILD_STDIO_CONVERT=N")==0)
        	|| (strcmp(envv[envIndex], "QIBM_JAVA_STDIO_CONVERT=N")==0)
        	|| (strcmp(envv[envIndex], "PASE_QIBM_JAVA_CHILD_STDIO_CONVERT=N")==0)) {

        	envv[envIndex][strlen(envv[envIndex])-1] = 'Y';

			if (DEBUG_FLAG) {
                printf("envIndex: %d, new value: %s\n", envIndex, envv[envIndex]);
			}

			envIndex++;
			continue;
		}

        //QIBM_PASE_DESCRIPTOR_STDIO: B is for binary data. and T is for text.
        //The default value is T, will cause translation of EBCDIC to ASCII.
        //But when inherit all environment variables from parent process,
        //It will be changed to be same with QIBM_JAVA_EXEC_CONV (B)
        //So Once output to STDOUT or STDERR, set it to be 'T'.
        if ( (strcmp(envv[envIndex], "QIBM_PASE_DESCRIPTOR_STDIO=B")==0) ) {
			envv[envIndex][strlen(envv[envIndex])-1] = 'T';

            if (DEBUG_FLAG) {
				printf("envIndex: %d, new value: %s\n", envIndex, envv[envIndex]);
            }

            envIndex++;
            continue;
        }

        envIndex++;
    }

    //But it is possible to delete QIBM_JAVA_CHILD_STDIO_CONVERT by getOs400Env();
    //when child process will inherit the environment variables of system
    //Actually this is a fault, but to narrow down the impaction for this fix, so process this situation here.
    if (!javaChildExist) {

    	if (DEBUG_FLAG) {
            printf("Don't exist QIBM_JAVA_CHILD_STDIO_CONVERT \n");
        }

    	if (inherit) {
    		envIndex = 0;
    		while(envv[envIndex] != NULL) {
    			envIndex++;
    		}
            envv[envIndex] = getMallocText("QIBM_JAVA_CHILD_STDIO_CONVERT=Y", NULL, 0);
    	}
    }
}

/*
 * Create a child process
 * env: pointer to the environment parameter related with JNIEnv
 * stdin_fd:   file descriptor of PIPE used by child process to read input
 * stdout_fd:  file descriptor of PIPE used by child process to write output related with STDOUT
 * stderr_fd:  file descriptor of PIPE used by child process to write output related with STDERR
 * jobname:    job name used by ILE
 * chdir_path: directory path
 * argv:       argument for command
 * envv:       environment variable
 * inherit:    whether need to inherit ILE environment variable.
 */
static int SpawnProcess(JNIEnv *env, int stdin_fd, int stdout_fd,               //IBM-iseries_bringup
                        int stderr_fd, char *jobname, char *chdir_path,                       //IBM-iseries_bringup
                        char **argv, char **envv, int inherit)                 //IBM-iseries_bringup
{                                                                               //IBM-iseries_bringup
    /* the QYJ9NTVUTL XPF method */                                             //IBM-iseries_bringup
    char* methodName;                                                           //IBM-iseries_bringup

    if(inherit) {                                                               //IBM-iseries_bringup
        methodName = "Qyj9SpawnProcessWithIleEnv";                              //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    else {                                                                      //IBM-iseries_bringup
        methodName = "Qyj9SpawnProcess";                                        //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* This structure defines the base of the ILE arglist for calling _ILECALL. */ //IBM-iseries_bringup
    typedef struct {                                                            //IBM-iseries_bringup
         ILEarglist_base base;                                             //IBM-iseries_bringup
         int32  stdin_fd;                                                  //IBM-iseries_bringup
         int32  stdout_fd;                                                 //IBM-iseries_bringup
         int32  stderr_fd;                                                 //IBM-iseries_bringup
         int32  ccsid;                                                     //IBM-iseries_bringup
         ts64_t jobname;                                                   //IBM-iseries_bringup
         ts64_t chdir_path;                                                //IBM-iseries_bringup
         ts64_t args;                                                      //IBM-iseries_bringup
         ts64_t envs;                                                      //IBM-iseries_bringup
    } MethodArgs;                                                               //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* Define the signatures for the arguments */                               //IBM-iseries_bringup
    static arg_type_t signature[] = {                                           //IBM-iseries_bringup
        ARG_INT32,                                                         //IBM-iseries_bringup
        ARG_INT32,                                                         //IBM-iseries_bringup
        ARG_INT32,                                                         //IBM-iseries_bringup
        ARG_INT32,                                                         //IBM-iseries_bringup
        ARG_MEMTS64,                                                       //IBM-iseries_bringup
        ARG_MEMTS64,                                                       //IBM-iseries_bringup
        ARG_MEMTS64,                                                       //IBM-iseries_bringup
        ARG_MEMTS64,                                                       //IBM-iseries_bringup
        ARG_END                                                            //IBM-iseries_bringup
     };                                                                    //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* Initialize the arglist */                                                //IBM-iseries_bringup
    char buf2[sizeof(MethodArgs) + 16];                                         //IBM-iseries_bringup
    MethodArgs* margs = (MethodArgs*) ROUND_QUAD(buf2);                         //IBM-iseries_bringup
    memset(margs,0x00,sizeof(MethodArgs));                                      //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* Create the argument list */                                              //IBM-iseries_bringup
    margs->stdin_fd  = (stdin_fd == 0)? stdin_fd : getIleFD(env, stdin_fd);     //IBM-iseries_bringup
    margs->stdout_fd = (stdout_fd == 1)? stdout_fd : getIleFD(env, stdout_fd);  //IBM-iseries_bringup
    margs->stderr_fd = ( (stderr_fd == 2)||(stderr_fd == 1) )? stderr_fd : getIleFD(env, stderr_fd);  //IBM-iseries_bringup

    if (DEBUG_FLAG) {                                                           //IBM-iseries_bringup
    	printf("stdin_fd=%d,   stdout_fd=%d,   stderr_fd=%d\n",                 //IBM-iseries_bringup
    	                stdin_fd, stdout_fd, stderr_fd);                        //IBM-iseries_bringup

        printf("  ile_in=%d,     ile_out=%d,     ile_err=%d\n",                 //IBM-iseries_bringup
               margs->stdin_fd, margs->stdout_fd, margs->stderr_fd);            //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    margs->ccsid      = -1;     /* Current PASE CCSID *//*ibm@138341*/          //IBM-iseries_bringup
    margs->jobname    = (ts64_t) jobname;                                       //IBM-iseries_bringup
    margs->chdir_path = (ts64_t) chdir_path;                                    //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* args - Convert the PASE 32-bit addresses into 64-bit addresses */        //IBM-iseries_bringup
    int argc = 0;                                                               //IBM-iseries_bringup
    while (argv[argc] != NULL) {                                                //IBM-iseries_bringup
        argc++;                                                                 //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    ts64_t *args = (ts64_t*) malloc((argc+1)*sizeof(ts64_t));                   //IBM-iseries_bringup
    argc = 0;                                                                   //IBM-iseries_bringup
    while (argv[argc] != NULL) {                                                //IBM-iseries_bringup
        args[argc] = (ts64_t)argv[argc]; argc++;                                //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    args[argc] = NULL;                                                          //IBM-iseries_bringup
    Xj9_GETTS64M(args, argc);                                                   //IBM-iseries_bringup
    margs->args = (ts64_t) args; /* ILECALL will convert this one for us */     //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /*convert child process STDIO to JOB CCSID if have been set as INHERIT from parent process */
    if ( (margs->stdin_fd == 0) || (margs->stdout_fd == 1) || (margs->stderr_fd == 2) ) {
    	checkEnvAndReplace(envv, inherit);
    }

    /* envs - Convert the PASE 32-bit addresses into 64-bit addresses */        //IBM-iseries_bringup
    int envc = 0;                                                               //IBM-iseries_bringup
    while (envv[envc] != NULL) {                                                //IBM-iseries_bringup
        envc++;                                                                 //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    ts64_t *envs = (ts64_t*) malloc((envc+1)*sizeof(ts64_t));                   //IBM-iseries_bringup
    envc = 0;                                                                   //IBM-iseries_bringup
    while (envv[envc] != NULL) {                                                //IBM-iseries_bringup
        envs[envc] = (ts64_t)envv[envc]; envc++;                                //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    envs[envc] = NULL;                                                          //IBM-iseries_bringup
    Xj9_GETTS64M(envs, envc);                                                   //IBM-iseries_bringup
    margs->envs = (ts64_t) envs; /* ILECALL will convert this one for us */     //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    // Call ile                                                                 //IBM-iseries_bringup
    Xj9CallIleProcedure(methodName,                                             //IBM-iseries_bringup
               "QYJ9NTVUTL",                                       //IBM-iseries_bringup
               NULL,                                               //IBM-iseries_bringup
               XJ9_CALLILE_NO_JNIENV,                              //IBM-iseries_bringup
               (ILEarglist_base*)&(margs->base),                   //IBM-iseries_bringup
               signature,                                          //IBM-iseries_bringup
               RESULT_INT32);                                      //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    int pid = margs->base.result.s_int32.r_int32;                               //IBM-iseries_bringup
    if (pid < 0) { /* It is an errno returned from ILE */                       //IBM-iseries_bringup
         Throw_InternalError(env, -1*pid, -1, "spawnp failure");           //IBM-iseries_bringup
         return -1;                                                        //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    else {                                                                       //IBM-iseries_bringup
        return pid;                                                       //IBM-iseries_bringup
    }
}                                                                               //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
/********************************************************************           //IBM-iseries_bringup
* FUNCTION NAME: getOs400envv                                                   //IBM-iseries_bringup
*                                                                               //IBM-iseries_bringup
* DESCRIPTION: Return the i5/OS version of envv** to be used for                //IBM-iseries_bringup
*              this target job                                                  //IBM-iseries_bringup
*                                                                               //IBM-iseries_bringup
*********************************************************************/          //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
static char** getOs400envv(JNIEnv *env, const char* penvBlock, jint envc, int* inherit, int* malloc_index) { //IBM-iseries_bringup
    char ** envv = NULL;                                                        //IBM-iseries_bringup
    /* Number of environment variables that we check to make sure are specified in the child process.*/ //IBM-iseries_bringup
    int max_forced_envs = 9;                                                    //IBM-iseries_bringup
    /* envv current index, used for populating the array. */                    //IBM-iseries_bringup
    int index = 0;                                                              //IBM-iseries_bringup
    /* Number of the environment variables in the current process to be sent to the child process. */ //IBM-iseries_bringup
    int environCount = 0;                                                       //IBM-iseries_bringup
    /* Allow I5 Classic JVM compatibility? Default is NO. The difference is that Classic ALWAYS //IBM-iseries_bringup
     * inherited the environment variables from the current process.  The Sun Runtime.exec() API //IBM-iseries_bringup
     * and J9 observe that if the user enters any envvars of their own, no inherited envvars are //IBM-iseries_bringup
     * used and only the user supplied envvars go into the child process. */    //IBM-iseries_bringup
    /* inherit -> whether or not the child is inheriting the env */             //IBM-iseries_bringup
    int classic_comp = 0;                                                       //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* Don't really want customers to know about this, but here if necessary. */ //IBM-iseries_bringup
    if ((getenv("QIBM_EXEC_FORCE_ALLENV") != NULL))                             //IBM-iseries_bringup
        classic_comp = 1;                                                       //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* Internal debug flag */                                                   //IBM-iseries_bringup
    DEBUG_FLAG = (getenv("QIBM_QYJ9NTVUTL_DEBUG") != NULL) ? 1 : 0;             //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    if (classic_comp || (penvBlock == NULL)) {                                  //IBM-iseries_bringup
    /* Count up the number of environment variables defined in the current process. */ //IBM-iseries_bringup
        extern char ** _environ;                                                //IBM-iseries_bringup
        while ( _environ[environCount] != NULL ) environCount++;                //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* User entered + I5 OS specific envvars                                    //IBM-iseries_bringup
       + current envvars (if applicable) + 1 for QIBM_JAVA_CHILD_STDIO_CONVERT  //IBM-iseries_bringup
       + 1 for null terminator */                                               //IBM-iseries_bringup
    int malloc_size = envc + max_forced_envs + environCount + 2;                //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* Allocate storage for the environment variable array. envv is never NULL on I5 OS. */ //IBM-iseries_bringup
    if ((envv = (char **) malloc(malloc_size * sizeof(char*))) == NULL) {       //IBM-iseries_bringup
        JNU_ThrowNullPointerException(env, NULL);                               //IBM-iseries_bringup
         return NULL;                                                           //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* Priority in the envv array starts at index 0 as the lowest priority.     //IBM-iseries_bringup
     * So the current envvars are added first,                                  //IBM-iseries_bringup
     * followed by our I5 OS envvars, and last the user input envvars. */       //IBM-iseries_bringup
    /* Copy the pointers of the current process environment variables into the new envv array. */ //IBM-iseries_bringup
    if (classic_comp || (penvBlock == NULL)) {                                  //IBM-iseries_bringup
        extern char ** _environ;                                                //IBM-iseries_bringup
        const char* paseUnderscore = "PASE_";                                   //IBM-iseries_bringup
        *inherit = 1;                                                           //IBM-iseries_bringup
        for (int i = 0; i < environCount; i++){                                 //IBM-iseries_bringup
           /* these guys are just bad news ibm@130083 */                        //IBM-iseries_bringup
        if((0  == strncmp(_environ[i],"QIBM_USE_DESCRIPTOR_STDIO=", strlen("QIBM_USE_DESCRIPTOR_STDIO="))) || //IBM-iseries_bringup
           (0 == strncmp(_environ[i],"QIBM_JFROMPASE_EXEC_DONE=", strlen("QIBM_JFROMPASE_EXEC_DONE=")))   || //IBM-iseries_bringup
           (0 == strncmp(_environ[i],"IBM_JAVA_COMMAND_LINE=", strlen("IBM_JAVA_COMMAND_LINE=")))   || //IBM-iseries_bringup
           (0 == strncmp(_environ[i],"LDR_CNTRL=", strlen("LDR_CNTRL=")))   ||  //IBM-iseries_bringup
           (0 == strncmp(_environ[i],"JAVA_HOME=", strlen("JAVA_HOME=")))   ||  //IBM-iseries_bringup
           (0 == strncmp(_environ[i],"TERMINAL_TYPE=", strlen("TERMINAL_TYPE="))) //IBM-iseries_bringup
           ){                                                                   //IBM-iseries_bringup
            continue;                                                           //IBM-iseries_bringup
        }                                                                       //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
      /* prefix with "PASE_" (we free what we malloc'ed later) */               //IBM-iseries_bringup
      char* newValue = (char*)malloc(1+strlen(paseUnderscore)+strlen(_environ[i])); //IBM-iseries_bringup
      newValue[0] = 0x00;                                                       //IBM-iseries_bringup
      if((0 != strncmp(_environ[i],paseUnderscore, strlen(paseUnderscore))) &&  //IBM-iseries_bringup
         (0 != strncmp(_environ[i],"QIBM_PASE_DESCRIPTOR_STDIO=", strlen("QIBM_PASE_DESCRIPTOR_STDIO="))))  //IBM-iseries_bringup
           strcat(newValue, paseUnderscore);                                    //IBM-iseries_bringup
      strcat(newValue, _environ[i]);                                            //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
      /* Do not add if it is the same  previous variable added */               //IBM-iseries_bringup
      if( (index >=1) && (0 == strcmp(newValue,envv[index-1])) )                //IBM-iseries_bringup
          continue;                                                             //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
      envv[index++] = newValue;                                                 //IBM-iseries_bringup
  }                                                                             //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* forced env #1 : A switch to allow for exec of things like "ls"      */   //IBM-iseries_bringup
    /* to come back readable. Passed to child as QIBM_PASE_DESCRIPTOR_STDIO */  //IBM-iseries_bringup
    char* QIBM_JAVA_EXEC_CONV = getenv("QIBM_JAVA_EXEC_CONV");                  //IBM-iseries_bringup
    if(QIBM_JAVA_EXEC_CONV) {                                                   //IBM-iseries_bringup
        envv[index++] = getMallocText("QIBM_PASE_DESCRIPTOR_STDIO=",            //IBM-iseries_bringup
                                         QIBM_JAVA_EXEC_CONV, 1);               //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* forced env #2 :  JAVA_HOME - Make sure it's the same as the parent */    //IBM-iseries_bringup
    char* JAVA_HOME = getenv("JAVA_HOME");                                      //IBM-iseries_bringup
    if(JAVA_HOME) {                                                             //IBM-iseries_bringup
       envv[index++] = getMallocText("JAVA_HOME=", JAVA_HOME, strlen(JAVA_HOME)); //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* forced env #3 :  QIBM_JVM_ILE_HOME - Make sure it's the same as the parent */ //IBM-iseries_bringup
    char* QIBM_JVM_ILE_HOME = getenv("QIBM_JVM_ILE_HOME");                      //IBM-iseries_bringup
    if(QIBM_JVM_ILE_HOME) {                                                     //IBM-iseries_bringup
       envv[index++] = getMallocText("QIBM_JVM_ILE_HOME=",                      //IBM-iseries_bringup
                                    QIBM_JVM_ILE_HOME, strlen(QIBM_JVM_ILE_HOME)); //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* forced env #4 : SHELL - without this defined, shell scripts without a "#!" line cannot run */ //IBM-iseries_bringup
    envv[index++] = getMallocText("SHELL=/usr/bin/qsh", NULL, 0);               //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* forced env #5 */                                                         //IBM-iseries_bringup
    /* Moved before user env vars to allow user envvars to override */          //IBM-iseries_bringup
    /* QIBM_JAVA_STDIO_CONVERT - Informs pipes to convert between child and parent.Default is NO. */ //IBM-iseries_bringup
    /* Set QIBM_JAVA_STDIO_CONVERT if QIBM_JAVA_CHILD_STDIO_CONVERT is set in this process.*/ //IBM-iseries_bringup
    char stdio_conv_var[] = "QIBM_JAVA_STDIO_CONVERT=N";                        //IBM-iseries_bringup
    char *stdio_conv_ptr = getenv("QIBM_JAVA_CHILD_STDIO_CONVERT");             //IBM-iseries_bringup
    if (stdio_conv_ptr != NULL)                                                 //IBM-iseries_bringup
        stdio_conv_var[sizeof(stdio_conv_var)-2] = stdio_conv_ptr[0];           //IBM-iseries_bringup
    envv[index++] = getMallocText(stdio_conv_var, NULL, 0);                     //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* Copy the user environment variable pointers into envv. */                //IBM-iseries_bringup
    if (penvBlock != NULL) {                                                    //IBM-iseries_bringup
       char *p = (char*) penvBlock;                                             //IBM-iseries_bringup
       for (int i = 0; i < envc; i++) {                                         //IBM-iseries_bringup
          /* Invariant: p always points to the start of a C string. */          //IBM-iseries_bringup
          envv[index++] = getMallocText(p, NULL, 0);                            //IBM-iseries_bringup
          while (*(p++));                                                       //IBM-iseries_bringup
        }                                                                       //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* forced env #6 :  QIBM_MULTI_THREADED - Forces multi-thread capability, which is (obviously) required for Java. */ //IBM-iseries_bringup
    envv[index++] = getMallocText("QIBM_MULTI_THREADED=Y", NULL, 0);            //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* forced env #7 :  QIBM_USE_DESCRIPTOR_STDIO - Allows the redirection of input and output from the child process. */ //IBM-iseries_bringup
    envv[index++] = getMallocText("QIBM_USE_DESCRIPTOR_STDIO=Y", NULL, 0);      //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    /* forced env #8 :  TERMINAL_TYPE - States that pipes are attached to the standard file descriptors. */ //IBM-iseries_bringup
    envv[index++] = getMallocText("TERMINAL_TYPE=PIPELINE", NULL, 0);           //IBM-iseries_bringup

    envv[index++] = NULL; /*used to store QIBM_JAVA_CHILD_STDIO_CONVERT */
                                                                                //IBM-iseries_bringup
    /* No more mallocs past this point or cleanup will be faulty. */            //IBM-iseries_bringup
    *malloc_index = index;                                                      //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    envv[index] = NULL; /* Null terminate array of environment variables. */    //IBM-iseries_bringup
    printArray("getOs400envv PASE envv", (const char**) envv);                  //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    return envv;                                                                //IBM-iseries_bringup
}                                                                               //IBM-iseries_bringup
#endif                                                                          //IBM-iseries_bringup
/*ibm@JSE-8409 end*//*IBM-ios_bringup*/                                         //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
/*                                                                              //IBM-iseries_bringup
 * There are 4 possible strategies we might use to "fork":
 *
 * - fork(2).  Very portable and reliable but subject to
 *   failure due to overcommit (see the documentation on
 *   /proc/sys/vm/overcommit_memory in Linux proc(5)).
 *   This is the ancient problem of spurious failure whenever a large
 *   process starts a small subprocess.
 *
 * - vfork().  Using this is scary because all relevant man pages
 *   contain dire warnings, e.g. Linux vfork(2).  But at least it's
 *   documented in the glibc docs and is standardized by XPG4.
 *   http://www.opengroup.org/onlinepubs/000095399/functions/vfork.html
 *   On Linux, one might think that vfork() would be implemented using
 *   the clone system call with flag CLONE_VFORK, but in fact vfork is
 *   a separate system call (which is a good sign, suggesting that
 *   vfork will continue to be supported at least on Linux).
 *   Another good sign is that glibc implements posix_spawn using
 *   vfork whenever possible.  Note that we cannot use posix_spawn
 *   ourselves because there's no reliable way to close all inherited
 *   file descriptors.
 *
 * - clone() with flags CLONE_VM but not CLONE_THREAD.  clone() is
 *   Linux-specific, but this ought to work - at least the glibc
 *   sources contain code to handle different combinations of CLONE_VM
 *   and CLONE_THREAD.  However, when this was implemented, it
 *   appeared to fail on 32-bit i386 (but not 64-bit x86_64) Linux with
 *   the simple program
 *     Runtime.getRuntime().exec("/bin/true").waitFor();
 *   with:
 *     #  Internal Error (os_linux_x86.cpp:683), pid=19940, tid=2934639536
 *     #  Error: pthread_getattr_np failed with errno = 3 (ESRCH)
 *   We believe this is a glibc bug, reported here:
 *     http://sources.redhat.com/bugzilla/show_bug.cgi?id=10311
 *   but the glibc maintainers closed it as WONTFIX.
 *
 * - posix_spawn(). While posix_spawn() is a fairly elaborate and
 *   complicated system call, it can't quite do everything that the old
 *   fork()/exec() combination can do, so the only feasible way to do
 *   this, is to use posix_spawn to launch a new helper executable
 *   "jprochelper", which in turn execs the target (after cleaning
 *   up file-descriptors etc.) The end result is the same as before,
 *   a child process linked to the parent in the same way, but it
 *   avoids the problem of duplicating the parent (VM) process
 *   address space temporarily, before launching the target command.
 *
 * Based on the above analysis, we are currently using vfork() on
 * Linux and spawn() on other Unix systems, but the code to use clone()
 * and fork() remains.
 */


static void
setSIGCHLDHandler(JNIEnv *env)
{
    /* There is a subtle difference between having the signal handler
     * for SIGCHLD be SIG_DFL and SIG_IGN.  We cannot obtain process
     * termination information for child processes if the signal
     * handler is SIG_IGN.  It must be SIG_DFL.
     *
     * We used to set the SIGCHLD handler only on Linux, but it's
     * safest to set it unconditionally.
     *
     * Consider what happens if java's parent process sets the SIGCHLD
     * handler to SIG_IGN.  Normally signal handlers are inherited by
     * children, but SIGCHLD is a controversial case.  Solaris appears
     * to always reset it to SIG_DFL, but this behavior may be
     * non-standard-compliant, and we shouldn't rely on it.
     *
     * References:
     * http://www.opengroup.org/onlinepubs/7908799/xsh/exec.html
     * http://www.pasc.org/interps/unofficial/db/p1003.1/pasc-1003.1-132.html
     */
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDSTOP | SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) < 0)
       JNU_ThrowInternalError(env, TOASCII("Can't set SIGCHLD handler"));       //IBM-zos_bringup
}

static void*
xmalloc(JNIEnv *env, size_t size)
{
    void *p = malloc(size);
    if (p == NULL)
        JNU_ThrowOutOfMemoryError(env, NULL);
    return p;
}

#define NEW(type, n) ((type *) xmalloc(env, (n) * sizeof(type)))

/**
 * If PATH is not defined, the OS provides some default value.
 * Unfortunately, there's no portable way to get this value.
 * Fortunately, it's only needed if the child has PATH while we do not.
 */
static const char*
defaultPath(void)
{
#ifdef __solaris__
    /* These really are the Solaris defaults! */
    return (geteuid() == 0 || getuid() == 0) ?
        "/usr/xpg4/bin:/usr/ccs/bin:/usr/bin:/opt/SUNWspro/bin:/usr/sbin" :
        "/usr/xpg4/bin:/usr/ccs/bin:/usr/bin:/opt/SUNWspro/bin:";
#else
    return ":/bin:/usr/bin";    /* glibc */
#endif
}

static const char*
effectivePath(void)
{
    const char *s = getenv("PATH");
    return (s != NULL) ? s : defaultPath();
}

static int
countOccurrences(const char *s, char c)
{
    int count;
    for (count = 0; *s != '\0'; s++)
        count += (*s == c);
    return count;
}

static const char * const *
effectivePathv(JNIEnv *env)
{
    char *p;
    int i;
    const char *path = effectivePath();
    int count = countOccurrences(path, ':') + 1;
    size_t pathvsize = sizeof(const char *) * (count+1);
    size_t pathsize = strlen(path) + 1;
    const char **pathv = (const char **) xmalloc(env, pathvsize + pathsize);

    if (pathv == NULL)
        return NULL;
    p = (char *) pathv + pathvsize;
    memcpy(p, path, pathsize);
    /* split PATH by replacing ':' with NULs; empty components => "." */
    for (i = 0; i < count; i++) {
        char *q = p + strcspn(p, ":");
        pathv[i] = (p == q) ? "." : p;
        *q = '\0';
        p = q + 1;
    }
    pathv[count] = NULL;
    return pathv;
}

JNIEXPORT void JNICALL
Java_java_lang_UNIXProcess_init(JNIEnv *env, jclass clazz)
{
    parentPathv = effectivePathv(env);
    setSIGCHLDHandler(env);
}


#ifndef WIFEXITED
#define WIFEXITED(status) (((status)&0xFF) == 0)
#endif

#ifndef WEXITSTATUS
#define WEXITSTATUS(status) (((status)>>8)&0xFF)
#endif

#ifndef WIFSIGNALED
#define WIFSIGNALED(status) (((status)&0xFF) > 0 && ((status)&0xFF00) == 0)
#endif

#ifndef WTERMSIG
#define WTERMSIG(status) ((status)&0x7F)
#endif

/* Block until a child process exits and return its exit code.
   Note, can only be called once for any given pid. */
JNIEXPORT jint JNICALL
Java_java_lang_UNIXProcess_waitForProcessExit(JNIEnv* env,
                                              jobject junk,
                                              jint pid)
{
/*ibm@JSE-8409 start*//*IBM-ios start*/                                         //IBM-iseries_bringup
#if defined(AIX)                                                                //IBM-iseries_bringup
    if (isOs400(env)) {                                                         //IBM-iseries_bringup
        /* Get a pointer to the QYJ9NTVUTL XPF method */                        //IBM-iseries_bringup
        char* methodName = "Qyj9WaitForProcessExit";                            //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
        /* This structure defines the base of the ILE arglist for calling _ILECALL. */ //IBM-iseries_bringup
        typedef struct { ILEarglist_base base; int32 pid; } MethodArgs;         //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
        /* Define the signatures for the arguments */                           //IBM-iseries_bringup
        static arg_type_t signature[] = { ARG_INT32, ARG_END };                 //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
        /* Initialize the arglist */                                            //IBM-iseries_bringup
        char buf2[sizeof(MethodArgs) + 16];                                     //IBM-iseries_bringup
        MethodArgs* margs = (MethodArgs*) ROUND_QUAD(buf2);                     //IBM-iseries_bringup
        memset(margs,0x00,sizeof(MethodArgs));                                  //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
        /* Create the argument list */                                          //IBM-iseries_bringup
        margs->pid = pid;                                                       //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
        Xj9CallIleProcedure(methodName,                                         //IBM-iseries_bringup
                "QYJ9NTVUTL",                                                   //IBM-iseries_bringup
                NULL,                                                           //IBM-iseries_bringup
                XJ9_CALLILE_NO_JNIENV,                                          //IBM-iseries_bringup
                (ILEarglist_base*)margs,                                        //IBM-iseries_bringup
                signature,                                                      //IBM-iseries_bringup
                RESULT_INT32);                                                  //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
        int retStatus = margs->base.result.s_int32.r_int32;                     //IBM-iseries_bringup
        return retStatus;                                                       //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
/*ibm@JSE-8409 end*//*IBM-ios end*/                                             //IBM-iseries_bringup
    else {                                                                      //IBM-iseries_bringup
#endif                                                                          //IBM-iseries_bringup
    /* We used to use waitid() on Solaris, waitpid() on Linux, but
     * waitpid() is more standard, so use it on all POSIX platforms. */
    int status;
    /* Wait for the child process to exit.  This returns immediately if
       the child has already exited. */
    while (waitpid(pid, &status, 0) < 0) {
        switch (errno) {
        case ECHILD: return 0;
        case EINTR: break;
        default: return -1;
        }
    }

    if (WIFEXITED(status)) {
        /*
         * The child exited normally; get its exit code.
         */
        return WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        /* The child exited because of a signal.
         * The best value to return is 0x80 + signal number,
         * because that is what all Unix shells do, and because
         * it allows callers to distinguish between process exit and
         * process death by signal.
         * Unfortunately, the historical behavior on Solaris is to return
         * the signal number, and we preserve this for compatibility. */
#ifdef __solaris__
        return WTERMSIG(status);
#else
        return 0x80 + WTERMSIG(status);
#endif
    } else {
        /*
         * Unknown exit code; pass it through.
         */
        return status;
    }
#if defined(AIX)  /*ibm@JSE-8409 */                                             //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
#endif                                                                          //IBM-iseries_bringup
}

static const char *
getBytes(JNIEnv *env, jbyteArray arr)
{
    return arr == NULL ? NULL :
        (const char*) (*env)->GetByteArrayElements(env, arr, NULL);
}

static void
releaseBytes(JNIEnv *env, jbyteArray arr, const char* parr)
{
    if (parr != NULL)
        (*env)->ReleaseByteArrayElements(env, arr, (jbyte*) parr, JNI_ABORT);
}

static void
throwIOException(JNIEnv *env, int errnum, const char *defaultDetail)
{
    static const char * const format = "error=%d, %s";
    const char *detail = defaultDetail;
    char *errmsg;
    char tmpbuf[1024];
    jstring s;

    if (errnum != 0) {
        int ret = getErrorString(errnum, tmpbuf, sizeof(tmpbuf));
        if (ret != EINVAL)
            detail = tmpbuf;
    }
    /* ASCII Decimal representation uses 2.4 times as many bits as binary. */
    errmsg = NEW(char, strlen(format) + strlen(detail) + 3 * sizeof(errnum));
    if (errmsg == NULL)
        return;

    sprintf(errmsg, format, errnum, detail);

    /* 
     * For zOS platform, the errmsg is in EBCDIC format. Use TOASCII() to 
     * convert to ASCII before passing to JNU_NewStringPlatform.
     */
    s = JNU_NewStringPlatform(env, TOASCII(errmsg));
    
    if (s != NULL) {
    /* 
     * The class name should be in UTF-8 format while passing to 
     * JNU_NewObjectByName(). Resume convlit to ensure UTF-8 format for zOS.
     */
#ifdef __MVS__                                                                  
#pragma convlit(resume)  
#endif                                                            
        jobject x = JNU_NewObjectByName(env, "java/io/IOException",
                                        "(Ljava/lang/String;)V", s);
#ifdef __MVS__                                                                  
#pragma convlit(suspend)
#endif                                                              
        if (x != NULL)
            (*env)->Throw(env, x);
    }
    free(errmsg);
}

#ifdef DEBUG_PROCESS
/* Debugging process code is difficult; where to write debug output? */
static void
debugPrint(char *format, ...)
{
    FILE *tty = fopen("/dev/tty", "w");
    va_list ap;
    va_start(ap, format);
    vfprintf(tty, format, ap);
    va_end(ap);
    fclose(tty);
}
#endif /* DEBUG_PROCESS */

/*********************************************************************/         //IBM-zos_bringup
/* Function Name: Java_java_lang_UNIXProcess_statExecutable()        */         //IBM-zos_bringup
/*                                                                   */         //IBM-zos_bringup
/* Description:   Return 0 if it is executable && readable by this   */         //IBM-zos_bringup
/*                process,  -1 if no such file, -2 if it cannot be   */         //IBM-zos_bringup
/*                executed, and raise exception                      */         //IBM-zos_bringup
/*                                                         ibm@79344 */         //IBM-zos_bringup
/*********************************************************************/         //IBM-zos_bringup
JNIEXPORT jint JNICALL Java_java_lang_UNIXProcess_statExecutable                //IBM-zos_bringup
                        (JNIEnv *env, jobject process, jbyteArray exename) {    //IBM-zos_bringup
    const char *pexename = getBytes(env, exename);                              //IBM-zos_bringup
    struct stat b;                                                              //IBM-zos_bringup
    char exe[MAXPATHLEN+1];                                                     //IBM-zos_bringup
    strncpy(exe,pexename,MAXPATHLEN+1);                                         //IBM-zos_bringup
    releaseBytes(env,exename,pexename);                                         //IBM-zos_bringup
                                                                                //IBM-zos_bringup
   /* Check to see if the file exists */                                        //IBM-zos_bringup
   if (stat(exe, &b)) return -1;                                                //IBM-zos_bringup
                                                                                //IBM-zos_bringup
   /* Check to see if this is a directory */                                    //IBM-zos_bringup
   if (S_ISDIR(b.st_mode)) return -2;                                           //IBM-zos_bringup
                                                                                //IBM-zos_bringup
   /* check for user permissions */                                             //IBM-zos_bringup
   if (access(exe, R_OK | X_OK) == 0) { /* R_OK only applicable to z/OS */      //IBM-zos_bringup
      return 0;                                                                 //IBM-zos_bringup
   } else {                                                                     //IBM-zos_bringup
      return -2;                                                                //IBM-zos_bringup
   }                                                                            //IBM-zos_bringup
}                                                                               //IBM-zos_bringup

static void
copyPipe(int from[2], int to[2])
{
    to[0] = from[0];
    to[1] = from[1];
}

/* arg is an array of pointers to 0 terminated strings. array is terminated
 * by a null element.
 *
 * *nelems and *nbytes receive the number of elements of array (incl 0)
 * and total number of bytes (incl. 0)
 * Note. An empty array will have one null element
 * But if arg is null, then *nelems set to 0, and *nbytes to 0
 */
static void arraysize(const char * const *arg, int *nelems, int *nbytes)
{
    int i, bytes, count;
    const char * const *a = arg;
    char *p;
    int *q;
    if (arg == 0) {
        *nelems = 0;
        *nbytes = 0;
        return;
    }
    /* count the array elements and number of bytes */
    for (count=0, bytes=0; *a != 0; count++, a++) {
        bytes += strlen(*a)+1;
    }
    *nbytes = bytes;
    *nelems = count+1;
}

/* copy the strings from arg[] into buf, starting at given offset
 * return new offset to next free byte
 */
static int copystrings(char *buf, int offset, const char * const *arg) {
    char *p;
    const char * const *a;
    int count=0;

    if (arg == 0) {
        return offset;
    }
    for (p=buf+offset, a=arg; *a != 0; a++) {
        int len = strlen(*a) +1;
        memcpy(p, *a, len);
        p += len;
        count += len;
    }
    return offset+count;
}

/**
 * We are unusually paranoid; use of clone/vfork is
 * especially likely to tickle gcc/glibc bugs.
 */
#ifdef __attribute_noinline__  /* See: sys/cdefs.h */
__attribute_noinline__
#endif

#define START_CHILD_USE_CLONE 0  /* clone() currently disabled; see above. */

#ifdef START_CHILD_USE_CLONE
static pid_t
cloneChild(ChildStuff *c) {
#ifdef __linux__
#define START_CHILD_CLONE_STACK_SIZE (64 * 1024)
    /*
     * See clone(2).
     * Instead of worrying about which direction the stack grows, just
     * allocate twice as much and start the stack in the middle.
     */
    if ((c->clone_stack = malloc(2 * START_CHILD_CLONE_STACK_SIZE)) == NULL)
        /* errno will be set to ENOMEM */
        return -1;
    return clone(childProcess,
                 c->clone_stack + START_CHILD_CLONE_STACK_SIZE,
                 CLONE_VFORK | CLONE_VM | SIGCHLD, c);
#else
/* not available on Solaris / Mac */
    assert(0);
    return -1;
#endif
}
#endif

static pid_t
vforkChild(ChildStuff *c) {
    volatile pid_t resultPid;

    /*
     * We separate the call to vfork into a separate function to make
     * very sure to keep stack of child from corrupting stack of parent,
     * as suggested by the scary gcc warning:
     *  warning: variable 'foo' might be clobbered by 'longjmp' or 'vfork'
     */
    resultPid = vfork();

    if (resultPid == 0) {
        childProcess(c);
    }
    assert(resultPid != 0);  /* childProcess never returns */
    return resultPid;
}

static pid_t
forkChild(ChildStuff *c) {
    pid_t resultPid;

    /*
     * From Solaris fork(2): In Solaris 10, a call to fork() is
     * identical to a call to fork1(); only the calling thread is
     * replicated in the child process. This is the POSIX-specified
     * behavior for fork().
     */
    resultPid = fork();

    if (resultPid == 0) {
        childProcess(c);
    }
    assert(resultPid != 0);  /* childProcess never returns */
    return resultPid;
}

#if defined(__solaris__) || defined(_ALLBSD_SOURCE) || defined(_AIX)
static pid_t
spawnChild(JNIEnv *env, jobject process, ChildStuff *c, const char *helperpath) {
    pid_t resultPid;
    jboolean isCopy;
    int i, offset, rval, bufsize, magic;
    char *buf, buf1[16];
    char *hlpargs[2];
    SpawnInfo sp;

    /* need to tell helper which fd is for receiving the childstuff
     * and which fd to send response back on
     */
    snprintf(buf1, sizeof(buf1), "%d:%d", c->childenv[0], c->fail[1]);
    /* put the fd string as argument to the helper cmd */
    hlpargs[0] = buf1;
    hlpargs[1] = 0;

    /* Following items are sent down the pipe to the helper
     * after it is spawned.
     * All strings are null terminated. All arrays of strings
     * have an empty string for termination.
     * - the ChildStuff struct
     * - the SpawnInfo struct
     * - the argv strings array
     * - the envv strings array
     * - the home directory string
     * - the parentPath string
     * - the parentPathv array
     */
    /* First calculate the sizes */
    arraysize(c->argv, &sp.nargv, &sp.argvBytes);
    bufsize = sp.argvBytes;
    arraysize(c->envv, &sp.nenvv, &sp.envvBytes);
    bufsize += sp.envvBytes;
    sp.dirlen = c->pdir == 0 ? 0 : strlen(c->pdir)+1;
    bufsize += sp.dirlen;
    arraysize(parentPathv, &sp.nparentPathv, &sp.parentPathvBytes);
    bufsize += sp.parentPathvBytes;
    /* We need to clear FD_CLOEXEC if set in the fds[].
     * Files are created FD_CLOEXEC in Java.
     * Otherwise, they will be closed when the target gets exec'd */
    for (i=0; i<3; i++) {
        if (c->fds[i] != -1) {
            int flags = fcntl(c->fds[i], F_GETFD);
            if (flags & FD_CLOEXEC) {
                fcntl(c->fds[i], F_SETFD, flags & (~1));
            }
        }
    }

    rval = posix_spawn(&resultPid, helperpath, 0, 0, (char * const *) hlpargs, environ);

    if (rval != 0) {
        return -1;
    }

    /* now the lengths are known, copy the data */
    buf = NEW(char, bufsize);
    if (buf == 0) {
        return -1;
    }
    offset = copystrings(buf, 0, &c->argv[0]);
    offset = copystrings(buf, offset, &c->envv[0]);
    memcpy(buf+offset, c->pdir, sp.dirlen);
    offset += sp.dirlen;
    offset = copystrings(buf, offset, parentPathv);
    assert(offset == bufsize);

    magic = magicNumber();

    /* write the two structs and the data buffer */
    write(c->childenv[1], (char *)&magic, sizeof(magic)); // magic number first
    write(c->childenv[1], (char *)c, sizeof(*c));
    write(c->childenv[1], (char *)&sp, sizeof(sp));
    write(c->childenv[1], buf, bufsize);
    free(buf);

    /* In this mode an external main() in invoked which calls back into
     * childProcess() in this file, rather than directly
     * via the statement below */
    return resultPid;
}
#endif

/*
 * Start a child process running function childProcess.
 * This function only returns in the parent.
 */
static pid_t
startChild(JNIEnv *env, jobject process, ChildStuff *c, const char *helperpath) {
    switch (c->mode) {
      case MODE_VFORK:
        return vforkChild(c);
      case MODE_FORK:
        return forkChild(c);
#if defined(__solaris__) || defined(_ALLBSD_SOURCE) || defined(_AIX)
      case MODE_POSIX_SPAWN:
        return spawnChild(env, process, c, helperpath);
#endif
      default:
        return -1;
    }
}

/*ibm@JSE-8409 start*//*IBM-ios start*/                                         //IBM-iseries_bringup
#if defined(AIX)                                                                //IBM-iseries_bringup

/*
 * Get the file descriptor used by spawnp()
 * pipeFd: pipe file descriptor used for communication between parent and child process
 * stdFd:  standard input/output/error file descriptor
 * return value: file descriptor used by spawnp()
 */
static int getFdForSpawnp(int pipeFd, int stdFd) {
	return (stdFd == -1)? pipeFd : stdFd;
}


#ifdef __attribute_noinline__  /* See: sys/cdefs.h */                           //IBM-iseries_bringup
__attribute_noinline__                                                          //IBM-iseries_bringup
#endif                                                                          //IBM-iseries_bringup
static pid_t                                                                    //IBM-iseries_bringup
startOs400Child(JNIEnv *env, ChildStuff *c, int inherit, int malloc_index) {    //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
   /*This debug info should be deleted when integrating back*/                  //IBM-iseries_bringup
   if (DEBUG_FLAG) {                                                            //IBM-iseries_bringup
       printf ("in[0]=%d     in[1]=%d \n",c->in[0], c->in[1]);                  //IBM-iseries_bringup
       printf ("out[0]=%d    out[1]=%d\n",c->out[0], c->out[1]);                //IBM-iseries_bringup
       printf ("err[0]=%d    err[1]=%d\n",c->err[0], c->err[1]);                //IBM-iseries_bringup
       printf ("fail[0]=%d   fail[1]=%d\n",c->fail[0], c->fail[1]);             //IBM-iseries_bringup
       printf ("fds[0]=%d    fds[1]=%d   fds[2]=%d\n",c->fds[0], c->fds[1],c->fds[2]); //IBM-iseries_bringup
       printf ("inherit =%d  malloc_index = %d\n", inherit, malloc_index);      //IBM-iseries_bringup
       printf ("c->redirectErrorStream = %d\n", c->redirectErrorStream);        //IBM-iseries_bringup
   }                                                                            //IBM-iseries_bringup

   int fdin = getFdForSpawnp(c->in[0], c->fds[0]);
   int fdout = getFdForSpawnp(c->out[1], c->fds[1]);
   int fderr = getFdForSpawnp(c->err[1], c->fds[2]);
   if ( (fdin<0) || (fdout<0) ||(fderr<0)) {
       return -1;
   }

   //int inherit = 0;                                                           //IBM-iseries_bringup
   //int malloc_index = 0;                                                      //IBM-iseries_bringup
   int resultPid = -1;                                                          //IBM-iseries_bringup
   const char *pprog = c->argv[0];                                              //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
/*	Temporarily add the code here. Not sure how to handle it              //IBM-iseries_bringup
*   Wehen fdout == fderr, redirect err_stream to /dev/null                      //IBM-iseries_bringup
*                                                                               //IBM-iseries_bringup
*/                                                                              //IBM-iseries_bringup
   if (c->redirectErrorStream) {                                                //IBM-iseries_bringup
        if ( (closeSafely(fderr) == -1) ) {                          //IBM-iseries_bringup
           return -1;                                                           //IBM-iseries_bringup
        }
        fderr = fdout;
   }                                                                            //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
   /* Calculate what the child jobname should be. */                            //IBM-iseries_bringup
	 char *jobname = Get_Job_Name(env, c->pdir);                            //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
   /* This is the default */                                                    //IBM-iseries_bringup
	 if (Spawn_In_ILE(env)) {                                               //IBM-iseries_bringup
	    /* Spawn the child process in ILE using spawnp() */                 //IBM-iseries_bringup
	    resultPid = SpawnProcess(env, fdin, fdout, fderr, jobname, (char*)c-> pdir, //IBM-iseries_bringup
			                            (char**)c-> argv, (char**)c->envv, inherit); //IBM-iseries_bringup
	 }                                                                      //IBM-iseries_bringup
   else if ((resultPid = Xj9fork400(jobname, 0)) == 0) {                        //IBM-iseries_bringup
   	  childProcess(c);                                                   //IBM-iseries_bringup
   }                                                                            //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
	if (jobname != NULL) free(jobname);                                     //IBM-iseries_bringup
	{                                                                       //IBM-iseries_bringup
		return resultPid;                                               //IBM-iseries_bringup
		assert(resultPid != 0);  /* childProcess never returns */       //IBM-iseries_bringup
	}                                                                       //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    return 0;  /* Suppress warning "no return value from function" */           //IBM-iseries_bringup
}                                                                               //IBM-iseries_bringup
#endif                                                                          //IBM-iseries_bringup
/*ibm@JSE-8409 end*//*IBM-ios end*/                                             //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
JNIEXPORT jint JNICALL
Java_java_lang_UNIXProcess_forkAndExec(JNIEnv *env,
                                       jobject process,
                                       jint mode,
                                       jbyteArray helperpath,
                                       jbyteArray prog,
                                       jbyteArray argBlock, jint argc,
                                       jbyteArray envBlock, jint envc,
                                       jbyteArray dir,
                                       jintArray std_fds,
                                       jboolean redirectErrorStream)
{
    int errnum;
    int resultPid = -1;
    int in[2], out[2], err[2], fail[2], childenv[2];
    jint *fds = NULL;
    const char *phelperpath = NULL;
    const char *pprog = NULL;
    const char *pargBlock = NULL;
    const char *penvBlock = NULL;

    in[0] = in[1] = out[0] = out[1] = err[0] = err[1] = fail[0] = fail[1] = -1;
    childenv[0] = childenv[1] = -1;

    /*                                                                               //IBM-zos_fork_and_exec
     * On z/OS fork/exec does not work for a multi-threaded application. Also they   //IBM-zos_fork_and_exec
     * don't work correctly for applications running in Key 2. Hence we need to use  //IBM-zos_fork_and_exec
     * spawn.                                                                        //IBM-zos_fork_and_exec
     */                                                                              //IBM-zos_fork_and_exec
#ifdef __MVS__                                                                   //IBM-zos_fork_and_exec
#define SP_FD_NUM  3                                                             //IBM-zos_fork_and_exec
    int    sp_fd_cnt;                                                            //IBM-zos_fork_and_exec
    int    sp_fd_map[SP_FD_NUM];                                                 //IBM-zos_fork_and_exec
    struct __inheritance sp_inherit;                                             //IBM-zos_fork_and_exec
    const char **passedEnvv;                                                     //IBM-zos_fork_and_exec
    const char *pdir = NULL;                                                    //IBM-zos_fork_and_exec
    const char **envv = NULL;                                                   //IBM-zos_fork_and_exec
    const char **argv = NULL;                                                   //IBM-zos_fork_and_exec
                                                                                //IBM-zos_fork_and_exec
    assert(prog != NULL && argBlock != NULL);                                   //IBM-zos_fork_and_exec
    if ((pprog     = getBytes(env, prog))       == NULL) goto Catch;            //IBM-zos_fork_and_exec
    if ((pargBlock = getBytes(env, argBlock))   == NULL) goto Catch;            //IBM-zos_fork_and_exec
                                                                                //IBM-zos_fork_and_exec
    /* Convert pprog + pargBlock into a char ** argv */                         //IBM-zos_fork_and_exec
    if ((argv = NEW(const char *, argc + 2)) == NULL) goto Catch;               //IBM-zos_fork_and_exec
    argv[0] = pprog;                                                            //IBM-zos_fork_and_exec
    initVectorFromBlock(argv+1, pargBlock, argc);                               //IBM-zos_fork_and_exec
                                                                                //IBM-zos_fork_and_exec
    /* Convert penvBlock into a char ** envv */                                 //IBM-zos_fork_and_exec
    if (envBlock != NULL) {                                                     //IBM-zos_fork_and_exec
        if ((penvBlock = getBytes(env, envBlock))   == NULL) goto Catch;        //IBM-zos_fork_and_exec
        if ((envv = NEW(const char *, envc + 1)) == NULL) goto Catch;           //IBM-zos_fork_and_exec
        initVectorFromBlock(envv, penvBlock, envc);                             //IBM-zos_fork_and_exec
    }                                                                           //IBM-zos_fork_and_exec
                                                                                //IBM-zos_fork_and_exec
    if (dir != NULL) {                                                          //IBM-zos_fork_and_exec
        if ((pdir = getBytes(env, dir)) == NULL) goto Catch;                    //IBM-zos_fork_and_exec
    }                                                                           //IBM-zos_fork_and_exec
                                                                                //IBM-zos_fork_and_exec
                                                                                //IBM-zos_fork_and_exec
    assert(std_fds != NULL);                                                    //IBM-zos_fork_and_exec
    if ((fds = (*env)->GetIntArrayElements(env, std_fds, NULL)) == NULL)        //IBM-zos_fork_and_exec
        goto Catch;                                                             //IBM-zos_fork_and_exec
                                                                                //IBM-zos_fork_and_exec
    if ((fds[0] == -1 && pipe(in)  < 0) ||                                      //IBM-zos_fork_and_exec
        (fds[1] == -1 && pipe(out) < 0) ||                                      //IBM-zos_fork_and_exec
        (fds[2] == -1 && pipe(err) < 0) ||                                      //IBM-zos_fork_and_exec
        (pipe(fail) < 0)) {                                                     //IBM-zos_fork_and_exec
        throwIOException(env, errno, TOASCII("Bad file descriptor"));            //IBM-zos_fork_and_exec
        goto Catch;                                                             //IBM-zos_fork_and_exec
        }                                                                       //IBM-zos_fork_and_exec
                                                                                //IBM-zos_fork_and_exec
    /* Map file descriptors for child.  This enables the child to use  */        //IBM-zos_fork_and_exec
    /* the literal values (0, 1, 2) as file descriptors.               */        //IBM-zos_fork_and_exec
    sp_fd_cnt    = SP_FD_NUM;                                                    //IBM-zos_fork_and_exec
    sp_fd_map[0] = (fds[0] == -1) ? in[0] : fds[0];                              //IBM-zos_fork_and_exec
    sp_fd_map[1] = (fds[1] == -1) ? out[1] : fds[1];                             //IBM-zos_fork_and_exec
    sp_fd_map[2] = (fds[2] == -1) ? err[1] : fds[2];                             //IBM-zos_fork_and_exec  
                                                                                 //IBM-zos_fork_and_exec
    /* Setup the current working directory for the process */                    //IBM-zos_fork_and_exec
    memset(&sp_inherit, 0x00, sizeof(sp_inherit));                              //IBM-zos_fork_and_exec
                                                                                //IBM-zos_fork_and_exec
    if (pdir != NULL) {                                                          //IBM-zos_fork_and_exec
        sp_inherit.flags = SPAWN_SETCWD;                                         //IBM-zos_fork_and_exec
        sp_inherit.cwdptr = (char *)pdir;                                        //IBM-zos_fork_and_exec
        sp_inherit.cwdlen = (int)strlen(sp_inherit.cwdptr);                      //IBM-zos_fork_and_exec
    }                                                                            //IBM-zos_fork_and_exec
                                                                                 //IBM-zos_fork_and_exec
    /* Ensure we have an environment setup for the child process */              //IBM-zos_fork_and_exec
    if (envv == NULL) {                                                          //IBM-zos_fork_and_exec
       extern char** environ;                                                   //IBM-zos_fork_and_exec
       passedEnvv = (const char **)environ;                                     //IBM-zos_fork_and_exec
    } else {                                                                     //IBM-zos_fork_and_exec
        passedEnvv = envv;                                                       //IBM-zos_fork_and_exec
    }                                                                            //IBM-zos_fork_and_exec
                                                                                 //IBM-zos_fork_and_exec
       if(redirectErrorStream) {                                                    //IBM-zos_fork_and_exec
        dup2(sp_fd_map[1],sp_fd_map[2]);                                        //IBM-zos_fork_and_exec
       }                                                                       //IBM-zos_fork_and_exec
                                                                                //IBM-zos_fork_and_exec
    /* Spawn child process */                                                    //IBM-zos_fork_and_exec
    resultPid = __spawnp2((const char *) argv[0],                                //IBM-zos_fork_and_exec
                               sp_fd_cnt,                                      //IBM-zos_fork_and_exec
                            sp_fd_map,                                          //IBM-zos_fork_and_exec
                            &sp_inherit,                                        //IBM-zos_fork_and_exec
                            argv,                                               //IBM-zos_fork_and_exec
                            passedEnvv);                                        //IBM-zos_fork_and_exec
                                                                                //IBM-zos_fork_and_exec
    if (resultPid < 0) {                                                         //IBM-zos_fork_and_exec
        JNU_ThrowIOExceptionWithLastError(env, strerror(errno));                 //IBM-zos_fork_and_exec
        goto Catch;                                                              //IBM-zos_fork_and_exec
    }                                                                            //IBM-zos_fork_and_exec
#else                                                                            //IBM-zos_fork_and_exec
    ChildStuff *c;                                                              //IBM-zos_fork_and_exec
                                                                                //IBM-zos_fork_and_exec
    if ((c = NEW(ChildStuff, 1)) == NULL) return -1;
    c->argv = NULL;
    c->envv = NULL;
    c->pdir = NULL;
    c->clone_stack = NULL;

    /* Convert prog + argBlock into a char ** argv.
     * Add one word room for expansion of argv for use by
     * execve_as_traditional_shell_script.
     * This word is also used when using spawn mode
     */
    assert(prog != NULL && argBlock != NULL);
    if ((phelperpath = getBytes(env, helperpath))   == NULL) goto Catch;
    if ((pprog       = getBytes(env, prog))         == NULL) goto Catch;
    if ((pargBlock   = getBytes(env, argBlock))     == NULL) goto Catch;
    if ((c->argv     = NEW(const char *, argc + 3)) == NULL) goto Catch;
    c->argv[0] = pprog;
    c->argc = argc + 2;
    initVectorFromBlock(c->argv+1, pargBlock, argc);

/*ibm@JSE-8409 start*//*IBM-ios start*/                                         //IBM-iseries_bringup
    int inherit = 0;                                                            //IBM-iseries_bringup
    int malloc_index = 0;                                                       //IBM-iseries_bringup
#if defined(AIX)                                                                //IBM-iseries_bringup
    if (isOs400(env)) {                                                         //IBM-iseries_bringup
    	 if (envBlock != NULL) {                                            //IBM-iseries_bringup
          /* Convert envBlock into a char ** envv */                            //IBM-iseries_bringup
          if ((penvBlock = getBytes(env, envBlock))   == NULL) goto Catch;      //IBM-iseries_bringup
       }                                                                        //IBM-iseries_bringup
	     c->envv = (const char**) getOs400envv(env, penvBlock, envc, &inherit, &malloc_index); //IBM-iseries_bringup
	     if (c->envv == NULL)                                               //IBM-iseries_bringup
	          goto Catch;                                                   //IBM-iseries_bringup
	     printArray("forkAndExec argv", c->argv);                           //IBM-iseries_bringup
	     printArray("forkAndExec envv", c->envv);                           //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    } else                                                                      //IBM-iseries_bringup
#endif                                                                          //IBM-iseries_bringup
/*ibm@JSE-8409 end*//*IBM-ios end*/                                             //IBM-iseries_bringup
    {                                                                           //IBM-iseries_bringup
        if (envBlock != NULL) {                                                 //IBM-iseries_bringup
            /* Convert envBlock into a char ** envv */                          //IBM-iseries_bringup
            if ((penvBlock = getBytes(env, envBlock))   == NULL) goto Catch;    //IBM-iseries_bringup
            if ((c->envv = NEW(const char *, envc + 1)) == NULL) goto Catch;    //IBM-iseries_bringup
            initVectorFromBlock(c->envv, penvBlock, envc);                      //IBM-iseries_bringup
        }                                                                       //IBM-iseries_bringup
    }

    if (dir != NULL) {
        if ((c->pdir = getBytes(env, dir)) == NULL) goto Catch;
    }

    assert(std_fds != NULL);
    fds = (*env)->GetIntArrayElements(env, std_fds, NULL);
    if (fds == NULL) goto Catch;

    if ((fds[0] == -1 && pipe(in)  < 0) ||
        (fds[1] == -1 && pipe(out) < 0) ||
        (fds[2] == -1 && pipe(err) < 0) ||
        (pipe(childenv) < 0) ||
        (pipe(fail) < 0)) {
        throwIOException(env, errno, "Bad file descriptor");           
        goto Catch;
    }

#if defined(AIX)
    if(isOs400(env)){
    	if ((fds[0] != -1)&&(in[0] == -1)&&(in[1] == -1)) {
			if (pipe(in) < 0) {
				throwIOException(env, errno, "Bad file descriptor for stdin");
				goto Catch;
			}
		}
        if ((fds[1] != -1)&&(out[0] == -1)&&(out[1] == -1)) {
        	if (pipe(out) < 0) {
        		throwIOException(env, errno, "Bad file descriptor for stdout");
        		goto Catch;
        	}
        }
        if ((fds[2] != -1)&&(err[0] == -1)&&(err[1] == -1)) {
        	if (pipe(err)<0) {
        		throwIOException(env, errno, "Bad file descriptor for stderr");
        		goto Catch;
        	}
        }
    }
#endif

    c->fds[0] = fds[0];
    c->fds[1] = fds[1];
    c->fds[2] = fds[2];

    copyPipe(in,   c->in);
    copyPipe(out,  c->out);
    copyPipe(err,  c->err);
    copyPipe(fail, c->fail);
    copyPipe(childenv, c->childenv);

    c->redirectErrorStream = redirectErrorStream;
    c->mode = mode;

/*ibm@JSE-8409 start*//*IBM-ios start*/                                         //IBM-iseries_bringup
#if defined(AIX)                                                                //IBM-iseries_bringup
    if(isOs400(env)){                                                           //IBM-iseries_bringup
        resultPid = startOs400Child(env,c,inherit,malloc_index);                 //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    else                                                                        //IBM-iseries_bringup
#endif                                                                          //IBM-iseries_bringup
    {                                                                           //IBM-iseries_bringup
        resultPid = startChild(env, process, c, phelperpath);                  //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
/*ibm@JSE-8409 end*//*IBM-ios end*/                                             //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    assert(resultPid != 0);

    if (resultPid < 0) {
        switch (c->mode) {
          case MODE_VFORK:
            throwIOException(env, errno, "vfork failed");
            break;
          case MODE_FORK:
            throwIOException(env, errno, "fork failed");
            break;
          case MODE_POSIX_SPAWN:
            throwIOException(env, errno, TOASCII("spawn failed"));
            break;
        }
        goto Catch;
    }
    close(fail[1]); fail[1] = -1; /* See: WhyCantJohnnyExec  (childproc.c)  */

    switch (readFully(fail[0], &errnum, sizeof(errnum))) {
    case 0: break; /* Exec succeeded */
    case sizeof(errnum):
        waitpid(resultPid, NULL, 0);
        throwIOException(env, errnum, TOASCII("Exec failed"));                  //IBM-zos_fork_and_exec
        goto Catch;
    default:
        throwIOException(env, errno, TOASCII("Read failed"));                   //IBM-zos_fork_and_exec
        goto Catch;
    }
#endif /* #ifdef __MVS__ */                                                      //IBM-zos_fork_and_exec

    fds[0] = (in [1] != -1) ? in [1] : -1;
    fds[1] = (out[0] != -1) ? out[0] : -1;
    fds[2] = (err[0] != -1) ? err[0] : -1;

 Finally:
#ifndef __MVS__
    free(c->clone_stack);
#endif /* #ifndef __MVS__ */

    /* Always clean up the child's side of the pipes */
    closeSafely(in [0]);
    closeSafely(out[1]);
    closeSafely(err[1]);

    /* Always clean up fail and childEnv descriptors */
    closeSafely(fail[0]);
    closeSafely(fail[1]);
    closeSafely(childenv[0]);
    closeSafely(childenv[1]);



    releaseBytes(env, prog,     pprog);
    releaseBytes(env, argBlock, pargBlock);
    releaseBytes(env, envBlock, penvBlock);
                                                                                //IBM-zos_fork_and_exec
#ifdef __MVS__                                                                  //IBM-zos_fork_and_exec
    releaseBytes(env, dir,      pdir);                                          //IBM-zos_fork_and_exec
                                                                                //IBM-zos_fork_and_exec
    free(argv);                                                                 //IBM-zos_fork_and_exec
    free(envv);                                                                 //IBM-zos_fork_and_exec
#else                                                                            //IBM-zos_fork_and_exec
    releaseBytes(env, dir,      c->pdir);
    releaseBytes(env,helperpath,phelperpath);


    free(c->argv);
/*ibm@JSE-8409 start*//*IBM-ios start*/                                         //IBM-iseries_bringup
#if defined(AIX)                                                                //IBM-iseries_bringup
    if (isOs400(env)) {                                                         //IBM-iseries_bringup
       for(int i = 0; i < malloc_index; i++) {                                  //IBM-iseries_bringup
           free((char*)(c->envv[i]));                                           //IBM-iseries_bringup
       }                                                                        //IBM-iseries_bringup
       if (c->envv    != NULL)                                                  //IBM-iseries_bringup
           free(c->envv);                                                       //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    else                                                                        //IBM-iseries_bringup
#endif                                                                          //IBM-iseries_bringup
    {                                                                           //IBM-iseries_bringup
       free(c->envv);                                                           //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
/*ibm@JSE-8409 end*//*IBM-ios end*/                                             //IBM-iseries_bringup
    free(c);
#endif /* #ifdef __MVS__ */                                                     //IBM-zos_fork_and_exec

    if (fds != NULL)
        (*env)->ReleaseIntArrayElements(env, std_fds, fds, 0);

    return resultPid;

 Catch:
    /* Clean up the parent's side of the pipes in case of failure only */
    closeSafely(in [1]); in[1] = -1;
    closeSafely(out[0]); out[0] = -1;
    closeSafely(err[0]); err[0] = -1;
    goto Finally;
}

JNIEXPORT void JNICALL
Java_java_lang_UNIXProcess_destroyProcess(JNIEnv *env,
                                          jobject junk,
                                          jint pid,
                                          jboolean force)
{
/*ibm@JSE-8409 start*//*IBM-ios start*/                                         //IBM-iseries_bringup
#if defined(AIX)                                                                //IBM-iseries_bringup
    if (isOs400(env)) {                                                         //IBM-iseries_bringup
        /* the QYJ9NTVUTL XPF method */                                         //IBM-iseries_bringup
	      char* methodName = "Qyj9DestroyProcess";                          //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
        /* This structure defines the base of the ILE arglist for calling _ILECALL. */ //IBM-iseries_bringup
	      typedef struct { ILEarglist_base base; int32 pid; } MethodArgs;   //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
        /* Define the signatures for the arguments */                           //IBM-iseries_bringup
	      static arg_type_t signature[] = { ARG_INT32, ARG_END };           //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
        /* Initialize the arglist */                                            //IBM-iseries_bringup
	      char buf2[sizeof(MethodArgs) + 16];                               //IBM-iseries_bringup
	      MethodArgs* margs = (MethodArgs*) ROUND_QUAD(buf2);               //IBM-iseries_bringup
	      memset(margs,0x00,sizeof(MethodArgs));                            //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
        /* Create the argument list */                                          //IBM-iseries_bringup
	      margs->pid = pid;                                                 //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
	      Xj9CallIleProcedure(methodName,                                   //IBM-iseries_bringup
	      		    "QYJ9NTVUTL",                                 //IBM-iseries_bringup
	      		    NULL,                                         //IBM-iseries_bringup
	      		    XJ9_CALLILE_NO_JNIENV,                        //IBM-iseries_bringup
	      		    (ILEarglist_base*)&(margs->base),             //IBM-iseries_bringup
	      		    signature,                                    //IBM-iseries_bringup
	      		    RESULT_VOID);                                 //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    else                                                                        //IBM-iseries_bringup
#endif                                                                          //IBM-iseries_bringup
/*ibm@JSE-8409 end*//*IBM-ios end*/                                             //IBM-iseries_bringup
   {                                                                            //IBM-iseries_bringup
    int sig = (force == JNI_TRUE) ? SIGKILL : SIGTERM;
    kill(pid, sig);
   }                                                                            //IBM-iseries_bringup
}
