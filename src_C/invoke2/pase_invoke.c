#include <jni.h>
#include <iconv.h>           
       
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

 #define PATH_SEPARATOR ';' /* define it to be ':' on Solaris */
 #define USER_CLASSPATH "." /* where Prog.class is */


 int main(int argc, char *argv[]) {
     JNIEnv *env;
     JavaVM *jvm;
     jint res;
     jclass cls;
     jmethodID mid,mainID;
     jstring jstr;
     jclass stringClass;
 	   jobjectArray argstr; 
     jclass  myClass;  
	   int inp=0;
	   char *arg819[3];
	   unsigned int len[3];
	   
 
   
 #ifdef JNI_VERSION_1_2
 
     JavaVMInitArgs vm_args;
     JavaVMOption options[1];
     printf("argv[1]=%s,argv[2]=%s\n",argv[1],argv[2]);
	    

     options[0].optionString =argv[1];
	 vm_args.version = 0x00010002;
     vm_args.options = options;
     vm_args.nOptions = 1;
     vm_args.ignoreUnrecognized = JNI_FALSE;
  

     res = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
	
 	#else 
    JDK1_1InitArgs vm_args;
     char classpath[1024];
     vm_args.version = 0x00010001;
     JNI_GetDefaultJavaVMInitArgs(&vm_args);
     /* Append USER_CLASSPATH to the default system class path */

     sprintf(classpath, "%s%c%s",
             vm_args.classpath, PATH_SEPARATOR, USER_CLASSPATH);
     printf("%s -- %c -- %s",vm_args.classpath,PATH_SEPARATOR,USER_CLASSPATH);
     vm_args.classpath = classpath;
     /* Create the Java VM */
 
      printf("argv[1]=%s,argv[2]=%s\n",argv[1],argv[2]);
     res = JNI_CreateJavaVM(&jvm, &env, &vm_args);
	 	 
 #endif /* JNI_VERSION_1_2 */
 	   if(res){
 	   		printf("Create JVM failed %d\n",res);
 	   		return 0;
 	   }
 	   		

    myClass = (*env)->FindClass(env,argv[2]);

    if (! myClass) {
       printf("Failed to find class");
      return 0;
    }
    

    mainID = (*env)->GetStaticMethodID(env,myClass,"main",
                                         "([Ljava/lang/String;)V");

    if (! mainID) {
      printf("Failed to find jmethodID of 'main'");
      return 0;
    }


    stringClass = (*env)->FindClass(env,"java/lang/String");

    if (! stringClass) {
       printf("Failed to find java/lang/String\n");
       return -1;
    }

    /*  Now, you need to create an empty array of strings,
     *  since main requires such an array as a parameter.
     */
    argstr =(*env)->NewObjectArray(env,0,stringClass,0);
    if (! argstr) {
       printf( "Failed to create args array\n");
       return (-1);
    }
    
    (*env)->CallStaticVoidMethod(env,myClass,mainID,argstr);

    

    /* Check for errors. */
    if ((*env)->ExceptionOccurred(env)) {
        (*env)->ExceptionDescribe(env);
    }
     
    if(argc<5 || strcmp(argv[4],"0")!=0){
     (*jvm)->DestroyJavaVM(jvm);
    }
     return(0);
 }
