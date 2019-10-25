/*
Author: Zhang Gan.
1) no need to convert. remove all converts.
2) Remove all QP2*, as it need to run under QP2TERM
3) compile command:
export JAVA_HOME=/QOpenSys/QIBM/ProdData/JavaVM/jdk80/64bit 
xlC_r -q64 -o invoke invokepase2.c -I$JAVA_HOME/include  -blibsuff:so -L$JAVA_HOME/jre/lib/ppc64/j9vm -ljvm
ldedit -brwexec_must a.out ===> This could let a.out be run with JIT enabled. Or else it would crash on JIT code.
*/
#include <jni.h>
#include <iconv.h>        
/*#include <qtqiconv.h>*/      
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 #define PATH_SEPARATOR ';' /* define it to be ':' on Solaris */
 #define USER_CLASSPATH "." /* where Prog.class is */

int convert(int oldccsid,int newccsid, char* olds, unsigned int oldlen, char **news, unsigned int* newlen){
	
	// convert the java home path to the pase ccsid.
			QtqCode_T   source;
			QtqCode_T   target;
			iconv_t 	cd;
			int rc;
			char *outbuff;
			unsigned int leftlen;
		  *newlen = (oldlen + 10) * 5;
#ifdef __LLP64_IFC__
			char *__ptr128 inBuf, *__ptr128 outBuf;
            //void *__ptr128 poldlen ,*__ptr128 pleftlen;
#else
            void * inBuf, *outBuf,*poldlen,*pleftlen;
#endif			
			memset(&target,0x00,sizeof(QtqCode_T));
			memset(&source,0x00,sizeof(QtqCode_T));
			source.CCSID = oldccsid;
			target.CCSID = newccsid;
			//  open the conversion
			cd = QtqIconvOpen(&target, &source);
			if (cd.return_value == -1) {
                               
				printf("error in conv\n");
				 return -1;
			}
		
			printf("malloc space for args ... %s:%d:%d:%d %d-> %d\n",olds,olds[0],olds[1],olds[2],oldlen,*newlen);
			*news = (char*)malloc(*newlen);
					printf("malloced space is %p\n",*news);
			leftlen=*newlen;
			outbuff=*news;
            inBuf = olds;
            outBuf = outbuff;
            // poldlen = &oldlen;
            // pleftlen = &leftlen;
			rc = iconv(cd, &inBuf,  &oldlen, &outBuf, &leftlen);
			(*news)[*newlen-leftlen]=0;
			*newlen-=leftlen;
			printf("convert is %d, 0:%d-> %d, 1:%d-> %d, 2:%d-> %d\n",*newlen,olds[0],(*news)[0],olds[1],(*news)[1],olds[2],(*news)[2]);
			if (rc != 0) {
                            printf("commonGetEnv error in iconv\n");
			    iconv_close(cd);
			    return -1;
			}    
	// close conversion
			iconv_close(cd);
			return 0;    
}			     
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
     
#ifdef __OS400_JNI_P128
	printf("zg.__OS400_JNI_P128 defined!\n");
#else
	printf("zg.__OS400_JNI_P128 NOT defined!\n"); 
#endif
#ifdef __LLP64_IFC__
	printf("zg.__LLP64_IFC__ defined!\n");
#else
	printf("zg.__LLP64_IFC__ NOT defined!\n"); 
#endif
   
 #ifdef JNI_VERSION_1_2
 
     JavaVMInitArgs vm_args;
     JavaVMOption options[1];
     printf("argv[1]=%s,argv[2]=%s\n",argv[1],argv[2]);
   
     if(convert(0,819,argv[1],strlen(argv[1]),&arg819[1],&len[1]) || convert(0,819,argv[2],strlen(argv[2]),&arg819[2],&len[2])){
        printf("Failed to convert args1\n");
            return -1;
     }

     options[0].optionString =arg819[1];
	 vm_args.version = 0x00010002;
     vm_args.options = options;
     vm_args.nOptions = 1;
     vm_args.ignoreUnrecognized = JNI_FALSE;
  
	   /* Create the Java VM */

     res = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
	
 	#else 
    JDK1_1InitArgs vm_args;
     char classpath[1024];
     vm_args.version = 0x00010001;
     JNI_GetDefaultJavaVMInitArgs(&vm_args);
     /* Append USER_CLASSPATH to the default system class path */
       #pragma convert(819)
     sprintf(classpath, "%s%c%s",
             vm_args.classpath, PATH_SEPARATOR, USER_CLASSPATH);
     printf("%s -- %c -- %s",vm_args.classpath,PATH_SEPARATOR,USER_CLASSPATH);
     vm_args.classpath = classpath;
     /* Create the Java VM */
     #pragma convert(0)
      printf("argv[1]=%s,argv[2]=%s\n",argv[1],argv[2]);
     res = JNI_CreateJavaVM(&jvm, &env, &vm_args);
	 	 
 #endif /* JNI_VERSION_1_2 */
 	   if(res){
 	   		printf("Create JVM failed %d\n",res);
 	   		return 0;
 	   }
 	   		
#pragma convert(819)
    myClass = (*env)->FindClass(env,arg819[2]);
#pragma convert(0)
    if (! myClass) {
       printf("Failed to find class");
      return 0;
    }
    
#pragma convert(819)
    mainID = (*env)->GetStaticMethodID(env,myClass,"main",
                                         "([Ljava/lang/String;)V");
#pragma convert(0)
    if (! mainID) {
      printf("Failed to find jmethodID of 'main'");
      return 0;
    }

#pragma convert(819)
    stringClass = (*env)->FindClass(env,"java/lang/String");
#pragma convert(0)
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
     
    if(argc<4 || strcmp(argv[3],"0")!=0){
     (*jvm)->DestroyJavaVM(jvm);
    }
     return(0);
 }
