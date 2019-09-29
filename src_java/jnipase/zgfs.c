/*========================================================================
 * IBM Confidential
 * OCO Source Materials
 *
 * IBM SDK, Java(tm) Technology Edition, v8
 * (C) Copyright IBM Corp. 1998, 2017. All Rights Reserved
 *
 * The source code for this program is not published or otherwise divested of
 * its trade secrets, irrespective of what has been deposited with the U.S.
 * Copyright office.
 *========================================================================
 */
/*
 * Copyright (c) 1998, 2012, Oracle and/or its affiliates. All rights reserved.
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

#include <assert.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <limits.h>

#include "jni.h"
#include "jni_util.h"
#include "jlong.h"
#include "jvm.h"
#include "io_util.h"
#include "io_util_md.h"
#include "java_io_FileSystem.h"
#include "java_io_UnixFileSystem.h"

#if defined(_ALLBSD_SOURCE)
#define dirent64 dirent
#define readdir64_r readdir_r
#define stat64 stat
#define statvfs64 statvfs
#endif

/* -- Field IDs -- */

static struct {
    jfieldID path;
} ids;

/*ibm@JSE-8409 start*//*IBM-ios start*/                                         //IBM-iseries_bringup
#if defined(AIX)		                                                //IBM-iseries_bringup
#define O_TEXTDATA  0100000000                                                  //IBM-iseries_bringup
#include "Xj9Interfaces.hpp"                                                    //IBM-iseries_bringup
#endif                                                                          //IBM-iseries_bringup
/*ibm@JSE-8409 end*//*IBM-ios end*/                                             //IBM-iseries_bringup

JNIEXPORT void JNICALL
Java_java_io_UnixFileSystem_initIDs1(JNIEnv *env, jclass cls)
{
	printf("zg.Java_java_io_UnixFileSystem_initIDs().CP0\n");
    jclass fileClass = (*env)->FindClass(env, "java/io/File");
    if (!fileClass) return;
    printf("zg.Java_java_io_UnixFileSystem_initIDs().CP1\n");
    ids.path = (*env)->GetFieldID(env, fileClass,
                                  "path", "Ljava/lang/String;");
}
JNIEXPORT void JNICALL
Java_java_io_UnixFileSystem_initIDs(JNIEnv *env, jclass cls)
{
	printf("zg.Java_java_io_UnixFileSystem_initIDs().CP0\n");
    jclass fileClass = (*env)->FindClass(env, "java/io/File");
    if (!fileClass) return;
    printf("zg.Java_java_io_UnixFileSystem_initIDs().CP1\n");
    ids.path = (*env)->GetFieldID(env, fileClass,
                                  "path", "Ljava/lang/String;");
}

/* -- Path operations -- */

extern int canonicalize(char *path, const char *out, int len);
/*ibm@JSE-8409 start*//*IBM-ios_bringup*/                                       //IBM-iseries_bringup
#if defined(AIX)                                                                //IBM-iseries_bringup
extern int os400_canonicalize(char *path, const char *out, int len); /*ibm@JSE-8409*/ //IBM-iseries_bringup
extern jboolean isOs400(JNIEnv *env); /*ibm@JSE-8409*/                          //IBM-iseries_bringup
#endif                                                                          //IBM-iseries_bringup
/*ibm@JSE-8409 end*//*IBM-ios_bringup*/                                         //IBM-iseries_bringup
JNIEXPORT jstring JNICALL
Java_java_io_UnixFileSystem_canonicalize0(JNIEnv *env, jobject this,
                                          jstring pathname)
{
    jstring rv = NULL;

    WITH_PLATFORM_STRING(env, pathname, path) {
        char canonicalPath[JVM_MAXPATHLEN];
/*ibm@JSE-8409 start*//*IBM-ios_bringup*/                                       //IBM-iseries_bringup
        int rc = -1;                                                            //IBM-iseries_bringup
#if defined(AIX)                                                                //IBM-iseries_bringup
        if(isOs400(env)) {                                                      //IBM-iseries_bringup
            rc = os400_canonicalize(JVM_NativePath((char*)path),                //IBM-iseries_bringup
                                    canonicalPath, JVM_MAXPATHLEN);             //IBM-iseries_bringup
        } else                                                                  //IBM-iseries_bringup
#endif                                                                          //IBM-iseries_bringup
        {                                                                       //IBM-iseries_bringup
            rc = canonicalize(JVM_NativePath((char*)path),                      //IBM-iseries_bringup
                              canonicalPath, JVM_MAXPATHLEN);                   //IBM-iseries_bringup
        }                                                                       //IBM-iseries_bringup
/*ibm@JSE-8409 end*//*IBM-ios_bringup*/                                         //IBM-iseries_bringup
        if ( rc < 0) { /*ibm@JSE-8409*/                                         //IBM-iseries_bringup
            JNU_ThrowIOExceptionWithLastError(env, "Bad pathname");
        } else {
#ifdef MACOSX
            rv = newStringPlatform(env, canonicalPath);
#else
            rv = JNU_NewStringPlatform(env, canonicalPath);
#endif
        }
    } END_PLATFORM_STRING(env, path);
    return rv;
}


/* -- Attribute accessors -- */

static jboolean
statMode(const char *path, int *mode)
{
#if defined(__MVS__)
	struct stat sb;
	if (lstat(path, &sb) == 0) {
		/*JSE-2652: Identifying if the path passed is an external link
         * Only applicable on zOS
         */
		if (S_ISEXTL(sb.st_mode, sb.st_genvalue) == 0) {
			if (stat(path, &sb) == 0) {
				*mode = sb.st_mode;
				return JNI_TRUE;
			}
		} else {
				*mode = sb.st_mode;
				return JNI_TRUE;
		}
	}
#else
    struct stat64 sb;
    if (stat64(path, &sb) == 0) {
        *mode = sb.st_mode;
        return JNI_TRUE;
    }
#endif
    return JNI_FALSE;
}

JNIEXPORT jint JNICALL
Java_java_io_UnixFileSystem_getBooleanAttributes1(JNIEnv *env, jobject this,
                                                  jobject file)
{
    jint rv = 0;
    printf("zg.Java_java_io_UnixFileSystem_getBooleanAttributes1.cp0\n");
    WITH_FIELD_PLATFORM_STRING(env, file, ids.path, path) {
        int mode;
        printf("zg.Java_java_io_UnixFileSystem_getBooleanAttributes1.path=%s\n",path);
        if (statMode(path, &mode)) {
            int fmt = mode & S_IFMT;
            rv = (jint) (java_io_FileSystem_BA_EXISTS
                  | ((fmt == S_IFREG) ? java_io_FileSystem_BA_REGULAR : 0)
                  | ((fmt == S_IFDIR) ? java_io_FileSystem_BA_DIRECTORY : 0));
        }
    } END_PLATFORM_STRING(env, path);
    return rv;
}

JNIEXPORT jint JNICALL
Java_java_io_UnixFileSystem_getBooleanAttributes0(JNIEnv *env, jobject this,
                                                  jobject file)
{
    jint rv = 0;

    WITH_FIELD_PLATFORM_STRING(env, file, ids.path, path) {
        int mode;
        if (statMode(path, &mode)) {
            int fmt = mode & S_IFMT;
            rv = (jint) (java_io_FileSystem_BA_EXISTS
                  | ((fmt == S_IFREG) ? java_io_FileSystem_BA_REGULAR : 0)
                  | ((fmt == S_IFDIR) ? java_io_FileSystem_BA_DIRECTORY : 0));
        }
    } END_PLATFORM_STRING(env, path);
    return rv;
}

JNIEXPORT jboolean JNICALL
Java_java_io_UnixFileSystem_checkAccess(JNIEnv *env, jobject this,
                                        jobject file, jint a)
{
    jboolean rv = JNI_FALSE;
    int mode = 0;
    switch (a) {
    case java_io_FileSystem_ACCESS_READ:
        mode = R_OK;
        break;
    case java_io_FileSystem_ACCESS_WRITE:
        mode = W_OK;
        break;
    case java_io_FileSystem_ACCESS_EXECUTE:
        mode = X_OK;
        break;
    default: assert(0);
    }
    WITH_FIELD_PLATFORM_STRING(env, file, ids.path, path) {
        if (access(path, mode) == 0) {
            rv = JNI_TRUE;
        }
    } END_PLATFORM_STRING(env, path);
    return rv;
}


JNIEXPORT jboolean JNICALL
Java_java_io_UnixFileSystem_setPermission(JNIEnv *env, jobject this,
                                          jobject file,
                                          jint access,
                                          jboolean enable,
                                          jboolean owneronly)
{
    jboolean rv = JNI_FALSE;

    WITH_FIELD_PLATFORM_STRING(env, file, ids.path, path) {
        int amode = 0;
        int mode;
        switch (access) {
        case java_io_FileSystem_ACCESS_READ:
            if (owneronly)
                amode = S_IRUSR;
            else
                amode = S_IRUSR | S_IRGRP | S_IROTH;
            break;
        case java_io_FileSystem_ACCESS_WRITE:
            if (owneronly)
                amode = S_IWUSR;
            else
                amode = S_IWUSR | S_IWGRP | S_IWOTH;
            break;
        case java_io_FileSystem_ACCESS_EXECUTE:
            if (owneronly)
                amode = S_IXUSR;
            else
                amode = S_IXUSR | S_IXGRP | S_IXOTH;
            break;
        default:
            assert(0);
        }
        if (statMode(path, &mode)) {
            if (enable)
                mode |= amode;
            else
                mode &= ~amode;
            if (chmod(path, mode) >= 0) {
                rv = JNI_TRUE;
            }
        }
    } END_PLATFORM_STRING(env, path);
    return rv;
}

JNIEXPORT jlong JNICALL
Java_java_io_UnixFileSystem_getLastModifiedTime(JNIEnv *env, jobject this,
                                                jobject file)
{
    jlong rv = 0;

    WITH_FIELD_PLATFORM_STRING(env, file, ids.path, path) {
        struct stat64 sb;
        if (stat64(path, &sb) == 0) {
#if defined(AIX)
            rv = (jlong)sb.st_mtime * 1000;
            rv += (jlong)sb.st_mtime_n / 1000000;
#elif defined(MACOSX)
            rv = (jlong)sb.st_mtimespec.tv_sec * 1000;
            rv += (jlong)sb.st_mtimespec.tv_nsec / 1000000;
#elif defined(__MVS__)
            rv = (jlong)sb.st_mtime * 1000;
#else
            rv = (jlong)sb.st_mtim.tv_sec * 1000;
            rv += (jlong)sb.st_mtim.tv_nsec / 1000000;
#endif
        }
    } END_PLATFORM_STRING(env, path);
    return rv;
}


JNIEXPORT jlong JNICALL
Java_java_io_UnixFileSystem_getLength(JNIEnv *env, jobject this,
                                      jobject file)
{
    jlong rv = 0;

    WITH_FIELD_PLATFORM_STRING(env, file, ids.path, path) {
        struct stat64 sb;
        if (stat64(path, &sb) == 0) {
            rv = sb.st_size;
        }
    } END_PLATFORM_STRING(env, path);
    return rv;
}


/* -- File operations -- */

/*ibm@JSE-8409 start*//*IBM-ios_bringup*/                                       //IBM-iseries_bringup
#if defined(AIX)                                                                //IBM-iseries_bringup
void os400_createFileExclusively_mode(int *mode, int *flags) {                  //IBM-iseries_bringup
    int file_auth=0;                                                            //IBM-iseries_bringup
    char * pname = "os400.file.create.auth";                                    //IBM-iseries_bringup
    char * Create_Auth_P =NULL;                                                 //IBM-iseries_bringup
    Create_Auth_P = (char *)Xj9GetPropertyValue(pname);                         //IBM-iseries_bringup
    if ((Create_Auth_P == NULL) || (strlen(Create_Auth_P)==0)) {                //IBM-iseries_bringup
        /* Set default value to Read and write for other (*PUBLIC) */           //IBM-iseries_bringup
        file_auth = file_auth | S_IROTH | S_IWOTH;                              //IBM-iseries_bringup
    } else {                                                                    //IBM-iseries_bringup
        /* deal with value of prop here */                                      //IBM-iseries_bringup
        for (int i=0;Create_Auth_P[i]!=0x00;i++) {                              //IBM-iseries_bringup
            if (Create_Auth_P[i] == 'R') { /* Add read for other (*PUBLIC) */   //IBM-iseries_bringup
                file_auth=file_auth|S_IROTH;                                    //IBM-iseries_bringup
            }                                                                   //IBM-iseries_bringup
            if (Create_Auth_P[i] == 'W') {  /* Add write for other (*PUBLIC) */ //IBM-iseries_bringup
                 file_auth=file_auth|S_IWOTH;                                   //IBM-iseries_bringup
            }                                                                   //IBM-iseries_bringup
            if (Create_Auth_P[i] == 'X') {  /* Add execute for other (*PUBLIC) */ //IBM-iseries_bringup
                file_auth=file_auth|S_IXOTH;                                    //IBM-iseries_bringup
            }                                                                   //IBM-iseries_bringup
        }                                                                       //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    *mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | file_auth;                  //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    char* Open_File_Text_P = (char *)Xj9GetPropertyValue(pname);                //IBM-iseries_bringup
    if ((Open_File_Text_P != NULL) && (strlen(Open_File_Text_P)!=0)) {          //IBM-iseries_bringup
	      if (0==strcmp(Open_File_Text_P,"TEXT")) {                         //IBM-iseries_bringup
            *flags = (*flags)|O_TEXTDATA;                                       //IBM-iseries_bringup
	      }                                                                 //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
}                                                                               //IBM-iseries_bringup
#endif                                                                          //IBM-iseries_bringup
/*ibm@JSE-8409 end*//*IBM-ios_bringup*/                                         //IBM-iseries_bringup

JNIEXPORT jboolean JNICALL
Java_java_io_UnixFileSystem_createFileExclusively(JNIEnv *env, jclass cls,
                                                  jstring pathname)
{
    jboolean rv = JNI_FALSE;

/*ibm@JSE-8409 start*//*IBM-ios_bringup*/                                       //IBM-iseries_bringup
    int mode = 0666;                                                            //IBM-iseries_bringup
    int flags = O_RDWR | O_CREAT | O_EXCL;                                      //IBM-iseries_bringup
#if defined(AIX)                                                                //IBM-iseries_bringup
    if (isOs400(env)) {                                                         //IBM-iseries_bringup
        os400_createFileExclusively_mode(&mode, &flags);                        //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
#endif                                                                          //IBM-iseries_bringup
/*ibm@JSE-8409 end*//*IBM-ios_bringup*/                                         //IBM-iseries_bringup
    WITH_PLATFORM_STRING(env, pathname, path) {
        int fd;
        if (!strcmp (path, "/")) {
            fd = JVM_EEXIST;    /* The root directory always exists */
        } else {
            fd = JVM_Open(path, flags, mode); /*ibm@JSE-8409*/                  //IBM-iseries_bringup
        }
        if (fd < 0) {
            if (fd != JVM_EEXIST) {
                JNU_ThrowIOExceptionWithLastError(env, path);
            }
        } else {
            JVM_Close(fd);
            rv = JNI_TRUE;
        }
    } END_PLATFORM_STRING(env, path);
    return rv;
}


JNIEXPORT jboolean JNICALL
Java_java_io_UnixFileSystem_delete0(JNIEnv *env, jobject this,
                                    jobject file)
{
    jboolean rv = JNI_FALSE;

    WITH_FIELD_PLATFORM_STRING(env, file, ids.path, path) {
        if (remove(path) == 0) {
            rv = JNI_TRUE;
        }
    } END_PLATFORM_STRING(env, path);
    return rv;
}

#if defined(AIX) /* 64-bit directory support */                                 //IBM-new_platforms
#define DIR DIR64                                                               //IBM-new_platforms
#define opendir opendir64                                                       //IBM-new_platforms
#define readdir readdir64                                                       //IBM-new_platforms
#define closedir closedir64                                                     //IBM-new_platforms
#define dirent dirent64                                                         //IBM-new_platforms
#endif                                                                          //IBM-new_platforms

JNIEXPORT jobjectArray JNICALL
Java_java_io_UnixFileSystem_list(JNIEnv *env, jobject this,
                                 jobject file)
{
    DIR *dir = NULL;
    struct dirent64 *ptr;
    struct dirent64 *result;
    int len, maxlen;
    jobjectArray rv, old;
    jclass str_class;

    str_class = JNU_ClassString(env);
    CHECK_NULL_RETURN(str_class, NULL);

    WITH_FIELD_PLATFORM_STRING(env, file, ids.path, path) {
        dir = opendir(path);
    } END_PLATFORM_STRING(env, path);
    if (dir == NULL) return NULL;

    ptr = malloc(sizeof(struct dirent64) + (PATH_MAX + 1));
    if (ptr == NULL) {
        JNU_ThrowOutOfMemoryError(env, "heap allocation failed");
        closedir(dir);
        return NULL;
    }

    /* Allocate an initial String array */
    len = 0;
    maxlen = 16;
    rv = (*env)->NewObjectArray(env, maxlen, str_class, NULL);
    if (rv == NULL) goto error;

    /* Scan the directory */
    while ((readdir64_r(dir, ptr, &result) == 0)  && (result != NULL)) {
        jstring name;
        if (!strcmp(ptr->d_name, ".") || !strcmp(ptr->d_name, ".."))
            continue;
        if (len == maxlen) {
            old = rv;
            rv = (*env)->NewObjectArray(env, maxlen <<= 1, str_class, NULL);
            if (rv == NULL) goto error;
            if (JNU_CopyObjectArray(env, rv, old, len) < 0) goto error;
            (*env)->DeleteLocalRef(env, old);
        }
#ifdef MACOSX
        name = newStringPlatform(env, ptr->d_name);
#else
        name = JNU_NewStringPlatform(env, ptr->d_name);
#endif
        if (name == NULL) goto error;
        (*env)->SetObjectArrayElement(env, rv, len++, name);
        (*env)->DeleteLocalRef(env, name);
    }
    closedir(dir);
    free(ptr);

    /* Copy the final results into an appropriately-sized array */
    old = rv;
    rv = (*env)->NewObjectArray(env, len, str_class, NULL);
    if (rv == NULL) {
        return NULL;
    }
    if (JNU_CopyObjectArray(env, rv, old, len) < 0) {
        return NULL;
    }
    return rv;

 error:
    closedir(dir);
    free(ptr);
    return NULL;
}

/*ibm@JSE-8409 start*//*IBM-ios_bringup*/                                       //IBM-iseries_bringup
#if defined(AIX)                                                                //IBM-iseries_bringup
void os400_createdirectory_mode(int *mode) {                                    //IBM-iseries_bringup
    int dir_auth=0;                                                             //IBM-iseries_bringup
    char * pname = "os400.dir.create.auth";                                     //IBM-iseries_bringup
    char * Create_Auth_P =NULL;                                                 //IBM-iseries_bringup
    Create_Auth_P = (char *)Xj9GetPropertyValue(pname);                         //IBM-iseries_bringup
    if ((Create_Auth_P == NULL) || (strlen(Create_Auth_P)==0)) {                //IBM-iseries_bringup
    /* Set default value to Read, write, and execute for other (*PUBLIC) */     //IBM-iseries_bringup
       dir_auth = dir_auth | S_IROTH | S_IWOTH | S_IXOTH;                       //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
    else {                                                                      //IBM-iseries_bringup
        /* deal with value of prop here */                                      //IBM-iseries_bringup
      for (int i=0;Create_Auth_P[i]!=0x00;i++) {                                //IBM-iseries_bringup
          if (Create_Auth_P[i] == 'R') {/* Add read for other (*PUBLIC)    */   //IBM-iseries_bringup
              dir_auth=dir_auth|S_IROTH;                                        //IBM-iseries_bringup
          }                                                                     //IBM-iseries_bringup
          if (Create_Auth_P[i] == 'W') {  /* Add write for other (*PUBLIC) */   //IBM-iseries_bringup
              dir_auth=dir_auth|S_IWOTH;                                        //IBM-iseries_bringup
          }                                                                     //IBM-iseries_bringup
          if (Create_Auth_P[i] == 'X') {  /* Add execute for other (*PUBLIC) */ //IBM-iseries_bringup
              dir_auth=dir_auth|S_IXOTH;                                        //IBM-iseries_bringup
          }                                                                     //IBM-iseries_bringup
      }                                                                         //IBM-iseries_bringup
   }                                                                            //IBM-iseries_bringup
                                                                                //IBM-iseries_bringup
    *mode = S_IRWXU | S_IRWXG | dir_auth;                                       //IBM-iseries_bringup
}                                                                               //IBM-iseries_bringup
#endif                                                                          //IBM-iseries_bringup
/*ibm@JSE-8409 end*//*IBM-ios_bringup*/                                         //IBM-iseries_bringup

JNIEXPORT jboolean JNICALL
Java_java_io_UnixFileSystem_createDirectory(JNIEnv *env, jobject this,
                                            jobject file)
{
    jboolean rv = JNI_FALSE;
/*ibm@JSE-8409 start*//*IBM-ios_bringup*/                                       //IBM-iseries_bringup
    int mode = 0777;                                                            //IBM-iseries_bringup
#if defined(AIX)                                                                //IBM-iseries_bringup
    if (isOs400(env)) {	                                                 //IBM-iseries_bringup
        os400_createdirectory_mode(&mode);                                      //IBM-iseries_bringup
    }                                                                           //IBM-iseries_bringup
#endif                                                                          //IBM-iseries_bringup
/*ibm@JSE-8409 end*//*IBM-ios_bringup*/                                         //IBM-iseries_bringup
    WITH_FIELD_PLATFORM_STRING(env, file, ids.path, path) {
        if (mkdir(path, mode) == 0) {/*ibm@JSE-8409*/                           //IBM-iseries_bringup
            rv = JNI_TRUE;
        }
    } END_PLATFORM_STRING(env, path);
    return rv;
}


JNIEXPORT jboolean JNICALL
Java_java_io_UnixFileSystem_rename0(JNIEnv *env, jobject this,
                                    jobject from, jobject to)
{
    jboolean rv = JNI_FALSE;

    WITH_FIELD_PLATFORM_STRING(env, from, ids.path, fromPath) {
        WITH_FIELD_PLATFORM_STRING(env, to, ids.path, toPath) {
            if (rename(fromPath, toPath) == 0) {
                rv = JNI_TRUE;
            }
        } END_PLATFORM_STRING(env, toPath);
    } END_PLATFORM_STRING(env, fromPath);
    return rv;
}

JNIEXPORT jboolean JNICALL
Java_java_io_UnixFileSystem_setLastModifiedTime(JNIEnv *env, jobject this,
                                                jobject file, jlong time)
{
    jboolean rv = JNI_FALSE;

    WITH_FIELD_PLATFORM_STRING(env, file, ids.path, path) {
        struct stat64 sb;

        if (stat64(path, &sb) == 0) {
            struct timeval tv[2];

            /* Preserve access time */
#if defined(AIX)
            tv[0].tv_sec = sb.st_atime;
            tv[0].tv_usec = sb.st_atime_n / 1000;
#elif defined(MACOSX)
            tv[0].tv_sec = sb.st_atimespec.tv_sec;
            tv[0].tv_usec = sb.st_atimespec.tv_nsec / 1000;
#elif defined(__MVS__)
            tv[0].tv_sec = sb.st_atime;
            tv[0].tv_usec = 0;
#else
            tv[0].tv_sec = sb.st_atim.tv_sec;
            tv[0].tv_usec = sb.st_atim.tv_nsec / 1000;
#endif

            /* Change last-modified time */
            tv[1].tv_sec = time / 1000;
            tv[1].tv_usec = (time % 1000) * 1000;

            if (utimes(path, tv) == 0)
                rv = JNI_TRUE;
        }
    } END_PLATFORM_STRING(env, path);

    return rv;
}


JNIEXPORT jboolean JNICALL
Java_java_io_UnixFileSystem_setReadOnly(JNIEnv *env, jobject this,
                                        jobject file)
{
    jboolean rv = JNI_FALSE;

    WITH_FIELD_PLATFORM_STRING(env, file, ids.path, path) {
        int mode;
        if (statMode(path, &mode)) {
            if (chmod(path, mode & ~(S_IWUSR | S_IWGRP | S_IWOTH)) >= 0) {
                rv = JNI_TRUE;
            }
        }
    } END_PLATFORM_STRING(env, path);
    return rv;
}

JNIEXPORT jlong JNICALL
Java_java_io_UnixFileSystem_getSpace(JNIEnv *env, jobject this,
                                     jobject file, jint t)
{
    jlong rv = 0L;

    WITH_FIELD_PLATFORM_STRING(env, file, ids.path, path) {
        struct statvfs64 fsstat;
        memset(&fsstat, 0, sizeof(fsstat));
        if (statvfs64(path, &fsstat) == 0) {
            switch(t) {
            case java_io_FileSystem_SPACE_TOTAL:
                rv = jlong_mul(long_to_jlong(fsstat.f_frsize),
                               long_to_jlong(fsstat.f_blocks));
                break;
            case java_io_FileSystem_SPACE_FREE:
                rv = jlong_mul(long_to_jlong(fsstat.f_frsize),
                               long_to_jlong(fsstat.f_bfree));
                break;
            case java_io_FileSystem_SPACE_USABLE:
                rv = jlong_mul(long_to_jlong(fsstat.f_frsize),
                               long_to_jlong(fsstat.f_bavail));
                break;
            default:
                assert(0);
            }
        }
    } END_PLATFORM_STRING(env, path);
    return rv;
}
