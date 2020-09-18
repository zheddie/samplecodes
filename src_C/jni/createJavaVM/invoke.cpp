#include <jni.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
using namespace std;
int main() {
    JNIEnv *env;
    JavaVM *jvm;
    JavaVMInitArgs vm_args;

    JavaVMOption options[5];
    char * p = NULL;
    jint res;
    jclass cls;
    jmethodID mid;
    jstring jstr;
    jobjectArray args;
    p = (char *)malloc(0x24);
    printf("&p=%p\n",p);
    memset(p,0x20,0x24);
    options[0].optionString = "-Xms4M";
    options[1].optionString = "-Xmx64M";
    options[2].optionString = "-Xss512K";
    //options[3].optionString = "-Xtrace:maximal={all},output=./invoketrace.trc";
    options[3].optionString = "-Xdump:java:events=vmstop";
    //options[3].optionString = "-Dibm.java9.forceCommonCleanerShutdown=true";
    options[4].optionString = "-Djava.class.path=.";
    vm_args.version = JNI_VERSION_1_4;
    vm_args.options = options;
    vm_args.nOptions = 5;
    vm_args.ignoreUnrecognized = JNI_FALSE;
    /* Create the Java VM */
    res = JNI_CreateJavaVM(&jvm,(void**)&env,&vm_args);
    printf("jvm=%p,env=%p\n",jvm,env);
    if (res < 0) {
        cerr<< "Can't create Java VM" << endl;
        return(-1);
    }
    cls = env->FindClass("Prog");
    printf("&cls=%p\n",&cls);
    if (cls == 0) {
        cerr << "Can't find Prog class" << endl;
        goto destroy;
    }
    mid = env->GetStaticMethodID(cls, "main", "([Ljava/lang/String;)V");
    printf("&mid=%p\n",mid);
    if (mid == 0) {
        cerr<< "Can't find Prog.main" << endl;
        goto destroy;
    }
    jstr = env->NewStringUTF(" from C++!");
    printf("&jstr=%p\n",jstr);
    if (jstr == 0) {
        cerr << "Out of memory" << endl;
        goto destroy;
    }
    args = env->NewObjectArray( 1,
                        env->FindClass("java/lang/String"), jstr);
    if (args == 0) {
        cerr << "Out of memory" << endl;
        goto destroy;
    }
    env->CallStaticVoidMethod( cls, mid, args);
destroy:
    if (env->ExceptionOccurred()) {
       env->ExceptionDescribe();
    }
    printf("env->NewObjectArray:%p\n",&JNIEnv::NewObjectArray);
    printf("DestroyJavaVM/pointer:%p\n",&JavaVM::DestroyJavaVM);
    //printf("DestroyJavaVM/pointer:%p\n",(void *)jvm->DestroyJavaVM);
    res=jvm->DestroyJavaVM();
    if (res != 0) {
        cerr<< "Failed to destroyed Java VM:res="<<res << endl;
        return(res);
    }
    return(0);
}
