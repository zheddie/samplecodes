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


static jboolean
statMode(const char *path, int *mode)
{

    printf("zgc.statMode.CP4,path=%s=\n",path);
    struct stat64 sb;
    int rt = stat64(path, &sb);
    //if (stat64(path, &sb) == 0) {
    if(0 == rt){
        printf("zgc.statMode.CP5\n");
        *mode = sb.st_mode;
        return JNI_TRUE;
    }
    printf("zgc.statMode.CP6,rt=%d\n",rt);

    return JNI_FALSE;
}

int getBooleanAttributes1(char * file)
{
    jint rv = 0;

        int mode;
        printf("zg.Java_java_io_UnixFileSystem_getBooleanAttributes1.CP1.path=%s\n",path);
        if (statMode(path, &mode)) {
            printf("zg.Java_java_io_UnixFileSystem_getBooleanAttributes1.CP2.mode=%d\n",mode);
            int fmt = mode & S_IFMT;
            rv = (jint) (java_io_FileSystem_BA_EXISTS
                  | ((fmt == S_IFREG) ? java_io_FileSystem_BA_REGULAR : 0)
                  | ((fmt == S_IFDIR) ? java_io_FileSystem_BA_DIRECTORY : 0));
        }
 
    return rv;
}