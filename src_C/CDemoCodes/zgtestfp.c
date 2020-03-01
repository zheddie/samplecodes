#define OS400_JVM_12
#define _MULTI_THREADED
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <jni.h>
/*
#include <QSYSINC/MIH/SETCA>
#include <QSYSINC/MIH/RETCA>
*/


#include <qp2shell2.h>
#include <pthread.h>
#include <time.h>

#define NEW_ATTRIBUTES 0x08000000 //division 0 enable attr

unsigned int old_attributes;
/* Specify the pragma that causes all literal strings in the
 * source code to be stored in ASCII (which, for the strings
 * used, is equivalent to UTF-8)
 */

#pragma convert(819)

/* Procedure: Oops
 *
 * Description: Helper routine that is called when a JNI function
 *              returns a zero value, indicating a serious error.
 *              This routine reports the exception to stderr and
 *              ends the JVM abruptly with a call to FatalError.
 *
 * Parameters:  env -- JNIEnv* to use for JNI calls
 *              msg -- char* pointing to error description in UTF-8
 *
 * Note:        Control does not return after the call to FatalError
 *              and it does not return from this procedure.
 */

void Oops(JNIEnv* env, char *msg) {
    if ((*env)->ExceptionOccurred(env)) {
        (*env)->ExceptionDescribe(env);
    }
    (*env)->FatalError(env, msg);
}

typedef struct {
   JavaVM* javaVM;
} threadparm_t;


void *theThread(void *parm)
{
   JavaVM* thJVM;           /* JavaVM pointer set by call to JNI_CreateJavaVM*/
   JNIEnv* thEnv;           /* JNIEnv pointer set by call to JNI_CreateJavaVM*/
   int               rc;
   threadparm_t     *gData;


    /*time_t theTime;*/

   char*   myClasspath;     /* Changeable classpath 'string' */
    jclass  myClass;         /* The class to call, 'NativeHello'. */
    jmethodID mainID;        /* The method ID of its 'main' routine. */
    jclass  stringClass;     /* Needed to create the String[] arg for main */
    jobjectArray args;       /* The String[] itself */

#pragma convert(0)
   printf("Thread %.16llx: Entered\n", pthread_getthreadid_np());
#pragma convert(819)

   gData = (threadparm_t *)parm;
   thJVM=gData->javaVM;

   //time(&theTime);

   (*thJVM)->AttachCurrentThread(thJVM, (void **)&thEnv, NULL);

 /* Save the prior exception mask attributes. */
/*    old_attributes = retca(_SRCA_EXCEPTION_MASK);
#pragma convert(0)
   printf("EXCEPTION MASK 1:%x\n", old_attributes);
#pragma convert(819)
*/




    /*  Use the newly created JVM to find the example class,
     *  called 'NativeHello'.
     */
    myClass = (*thEnv)->FindClass(thEnv, "zHello");
    if (! myClass) {
        Oops(thEnv, "Failed to find class 'Hello'");
    }

    /* Now, get the method identifier for the 'main' entry point
     * of the class.
     * Note: The signature of 'main' is always the same for any
     *       class called by the following java command:
     *           "main" , "([Ljava/lang/String;)V"
     */
    mainID = (*thEnv)->GetStaticMethodID(thEnv,myClass,"main",
                                         "([Ljava/lang/String;)V");
    if (! mainID) {
        Oops(thEnv, "Failed to find jmethodID of 'main'");
    }

    /* Get the jclass for String to create the array
     * of String to pass to 'main'.
     */
    stringClass = (*thEnv)->FindClass(thEnv, "java/lang/String");
    if (! stringClass) {
        Oops(thEnv, "Failed to find java/lang/String");
    }

    /*  Now, you need to create an empty array of strings,
     *  since main requires such an array as a parameter.
     */
    args = (*thEnv)->NewObjectArray(thEnv,0,stringClass,0);
    if (! args) {
        Oops(thEnv, "Failed to create args array");
    }




    /* Now, you have the methodID of main and the class, so you can
     * call the main method.
     */
    (*thEnv)->CallStaticVoidMethod(thEnv,myClass,mainID,args);



    /* Check for errors. */
    if ((*thEnv)->ExceptionOccurred(thEnv)) {
        (*thEnv)->ExceptionDescribe(thEnv);
    }
   (*thJVM)->DetachCurrentThread(thJVM);
   return NULL;
}


/* This is the program's "main" routine. */
int main (int argc, char *argv[])
{
    pthread_t             thread;
    int rc=0;
    threadparm_t          gData;
    JavaVMInitArgs initArgs;
    JavaVM* myJVM;           /* JavaVM pointer set by call to JNI_CreateJavaVM*/
    JNIEnv* myEnv;           /* JNIEnv pointer set by call to JNI_CreateJavaVM*/

    JavaVMOption options[1]; /* Options array -- use options to set classpath */
    int     fd0, fd1, fd2;   /* file descriptors for IO */
    double d1=1.0, d2=0.0;

    /* Open the file descriptors so that IO works. */
    fd0 = open("/dev/null", O_CREAT|O_TRUNC|O_RDWR,   S_IRUSR|S_IROTH);
    fd1 = open("/home/zhanggan/stdout.txt",
               O_CREAT|O_TRUNC|O_WRONLY, S_IWUSR|S_IWOTH);
    fd2 = open("/home/zhanggan/stderr.txt",
               O_CREAT|O_TRUNC|O_WRONLY, S_IWUSR|S_IWOTH);
    /*  Set the version field of the initialization arguments for JNI v1.5. */
    initArgs.version = 0x00010004;

  /*  setca(NEW_ATTRIBUTES, _SRCA_EXCEPTION_MASK); //enable division 0*/
    /*#pragma convert(0)
      char mydiv[]="/home/wenlint/mysrc/div0";
     QP2SHELL2(mydiv);
     #pragma convert(819)*/


    /* Now, you want to specify the directory for the class to run in the classp
     * with  Java2, classpath is passed in as an option.
     * Note: You must specify the directory name in UTF-8 format. So, you wrap
     *       blocks of code in #pragma convert statements.
     */
    options[0].optionString="-Djava.class.path=/HOME/zhanggan/";

    initArgs.options=options;  /* Pass in the classpath that has been set up. */
    initArgs.nOptions = 1;     /* Pass in classpath and version options */

    /*  Create the JVM -- a nonzero return code indicates there was
     *  an error. Drop back into EBCDIC and write a message to stderr
     *  before exiting the program.
     *  Note:  This will run the default JVM and JDK which is 32bit JDK 6.0.
     *  If you want to run a different JVM and JDK, set the JAVA_HOME environmen
     *  variable to the home directory of the JVM you want to use
     *  (prior to the CreateJavaVM() call).
     */
    if (JNI_CreateJavaVM(&myJVM, (void **)&myEnv, (void *)&initArgs)) {
		 		 #pragma convert(0)
        		 fprintf(stderr, "Failed to create the JVM\n");
		 		 #pragma convert(819)
        		 exit(1);
    }


    gData.javaVM=myJVM;
    rc = pthread_create(&thread, NULL, theThread, &gData);
    #pragma convert(0)
    printf("Create thread result:%d\n",rc);
    printf("Start to join:\n");
    #pragma convert(819)
    rc = pthread_join(thread, NULL);

    /* Finally, destroy the JavaVM that you created. */
    (*myJVM)->DestroyJavaVM(myJVM);
    #pragma convert(0)
    printf("Main completed\n");
    printf("EXCEPTION MASK :%x\n", old_attributes);
    #pragma convert(819)

    /* All done. */
    return 0;
}
