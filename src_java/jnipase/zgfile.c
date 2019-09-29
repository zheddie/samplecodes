/*
 *

 * This native method implements the getStringNative method of class
 * PaseExample1. It uses the JNI function CallObjectMethod to call 
 * back to the getStringCallback method of class PaseExample1.
 *
 * Compile this code in AIX or PASE for i to create module 'libPaseExample1.so'.
 *
 */
#include <stdlib.h>
#include <sys/stat.h>

#include "jni.h"
#include "jni_util.h"
#include "jlong.h"
#include "jvm.h"
#include "io_util.h"
#include "io_util_md.h"
#include "java_io_FileSystem.h"
//#include "java_io_UnixFileSystem.h"
#include "FileCheck.h"
static struct {
    jfieldID path;
} ids;

static jboolean
statMode(const char *path, int *mode)
{
#if defined(__MVS__)
    printf("zgc.statMode.CP0\n");
    struct stat sb;
    if (lstat(path, &sb) == 0) {
        /*JSE-2652: Identifying if the path passed is an external link
         * Only applicable on zOS
         */
        printf("zgc.statMode.CP1\n");
        if (S_ISEXTL(sb.st_mode, sb.st_genvalue) == 0) {
            if (stat(path, &sb) == 0) {
                printf("zgc.statMode.CP2\n");
                *mode = sb.st_mode;
                return JNI_TRUE;
            }
        } else {
            printf("zgc.statMode.CP3\n");
                *mode = sb.st_mode;
                return JNI_TRUE;
        }
    }
#else
    printf("zgc.statMode.CP4,path=%s=\n",path);
    struct stat64 sb;

    int rt = stat64(path, &sb);
    //if (stat64(path, &sb) == 0) {
    if(0 == rt){
        
        *mode = sb.st_mode;
        printf("zgc.statMode.CP5,mode=%d\n",*mode);
        return JNI_TRUE;
    }
    printf("zgc.statMode.CP6,rt=%d,errno=%d\n",rt,errno);
#endif
    printf("zgc.statMode.CP9\n");
    return JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_FileCheck_initIDs1(JNIEnv *env, jclass cls)
{
    printf("zg.Java_FileCheck_getBooleanAttributes1().CP0\n");
    jclass fileClass = (*env)->FindClass(env, "java/io/File");
    if (!fileClass) return;
    printf("zg.Java_FileCheck_getBooleanAttributes1().CP1\n");
    ids.path = (*env)->GetFieldID(env, fileClass,
                                  "path", "Ljava/lang/String;");
}

JNIEXPORT jint JNICALL
Java_FileCheck_getBooleanAttributes1(JNIEnv *env, jobject this,
                                                  jobject file)
{
    jint rv = 0;
    printf("zg.Java_FileCheck_getBooleanAttributes1.CP0\n");
    char * pccsid=getenv("QIBM_PASE_CCSID");
    printf("zg.Java_FileCheck_getBooleanAttributes1.pccsid=%X,*QIBM_PASE_CCSID=%s\n",pccsid,pccsid);
    char * plang=getenv("PASE_LANG");
    printf("zg.Java_FileCheck_getBooleanAttributes1.plang=%X,*PASE_LANG=%s\n",plang,plang);
    char * pl=getenv("LANG");
    printf("zg.Java_FileCheck_getBooleanAttributes1.pl=%X,*LANG=%s\n",pl,pl);
   
    printf("zg.getchar() for QDLS\n");
    getchar();
    struct stat64 sb;
    int rt = stat64("/QDLS", &sb);
   if(rt == 0){
        printf("zg.QDLS.Java_FileCheck_getBooleanAttributes1.CP0b.rt=0,mode=%d\n",sb.st_mode);
    }else{
        printf("zg.QDLS.Java_FileCheck_getBooleanAttributes1.CP0a.rt=%d\n",rt);
    }
    printf("zg.getchar() for qopensys\n");
    getchar();
    rt = stat64("/qopensys", &sb); 
    if(rt == 0){
        printf("zg.qopensys.Java_FileCheck_getBooleanAttributes1.CP0b.rt=0,mode=%d\n",sb.st_mode);
    }else{
        printf("zg.qopensys.Java_FileCheck_getBooleanAttributes1.CP0a.rt=%d\n",rt);
    }
    WITH_FIELD_PLATFORM_STRING(env, file, ids.path, path) {
        int mode;
        printf("zg.Java_FileCheck_getBooleanAttributes1.CP1.path=%s=\n",path);
        if (statMode(path, &mode)) {
            printf("zg.Java_FileCheck_getBooleanAttributes1.CP2.mode=%d\n",mode);
            int fmt = mode & S_IFMT;
            printf("zg.Java_FileCheck_getBooleanAttributes1.CP3.fmt=%d\n",fmt);
            rv = (jint) (java_io_FileSystem_BA_EXISTS
                  | ((fmt == S_IFREG) ? java_io_FileSystem_BA_REGULAR : 0)
                  | ((fmt == S_IFDIR) ? java_io_FileSystem_BA_DIRECTORY : 0));
        }
    } END_PLATFORM_STRING(env, path);
    return rv;
}