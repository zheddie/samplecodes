#include <jni.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
int main() {
    JNIEnv *env;
    JavaVM *jvm;
    JavaVMInitArgs vm_args;
    JavaVMOption options[5];
    jint res;
    jclass cls;
    jmethodID mid;
    jstring jstr;
    jobjectArray args;
     options[0].optionString = "-Xms4M";
     options[1].optionString = "-Xmx64M";
     options[2].optionString = "-Xss512K";
//     options[5].optionString = "-Xoss400K";
     options[4].optionString = "-Djava.class.path=.";
     options[3].optionString = "-Xtrace:print=none";

     vm_args.version = JNI_VERSION_1_4;
     vm_args.options = options;
     vm_args.nOptions = 5;
     vm_args.ignoreUnrecognized = JNI_FALSE;
    /* Create the Java VM */
    res = JNI_CreateJavaVM(&jvm,(void**)&env,&vm_args);
    if (res < 0) {
        cerr<< "Can't create Java VM" << endl;
        goto destroy;
    }
    cls = env->FindClass("Prog");
    if (cls == 0) {
        cerr << "Can't find Prog class" << endl;
        goto destroy;
    }
    mid = env->GetStaticMethodID(cls, "main", "([Ljava/lang/String;)V");
    if (mid == 0) {
        cerr<< "Can't find Prog.main" << endl;
        goto destroy;
    }
    jstr = env->NewStringUTF(" from C++!");
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
    jvm->DestroyJavaVM();
}
