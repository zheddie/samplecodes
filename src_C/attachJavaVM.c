#include <jni.h>
#include <iconv.h>           
#include <qtqiconv.h>        
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <qp2shell2.h>
#include <qp2user.h>
#include <pthread.h>
#define PATH_SEPARATOR ';' /* define it to be ':' on Solaris */
#define USER_CLASSPATH "." /* where Prog.class is */

#define PTHREAD_OPTION_CREATE_THREAD_NP  0x40C3D9E3  // " CRT" in hex
#define PTHREAD_OPTIONVALUE_SYSTEM_NP    0x40E2E8E2  // " SYS" in hex
#define PTHREAD_OPTIONVALUE_USER_NP      0x00000000  // User Thread


typedef struct {
	int   value;
	char  string[128];
} thread_parm_t;
JavaVM *jvm;
char *arg819[3];

jint res;
jclass cls;
jmethodID mid,mainID;
jstring jstr;
jclass stringClass;
jobjectArray argstr; 
jclass  myClass; 


void * checkResults(char * str,int val){
	printf("rt=%d,str=%s",val,str);
	return NULL;
}


int convert(int oldccsid,int newccsid, char* olds, unsigned int oldlen, char **news, unsigned int* newlen){

	// convert the java home path to the pase ccsid.
	QtqCode_T   source;
	QtqCode_T   target;
	iconv_t 	cd;
	int rc;
	char *outbuff;
	unsigned int leftlen;
	*newlen = (oldlen + 10) * 5;


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
	rc = iconv(cd, &olds, &oldlen, &outbuff, &leftlen);
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

void *threadfunc(void *parm)
{
	float test = 10.0;
	float zero = 0.0;
	JNIEnv *env;
	thread_parm_t *p = (thread_parm_t *)parm;
	printf("%s, parm = %d\n", p->string, p->value);
	test = test/zero;
	printf("zg.test=%f\n",test);
	free(p);

#ifdef JNI_VERSION_1_2
	res = (*jvm)->AttachCurrentThread(jvm, (void**)&env, NULL);

#else
	res = (*jvm)->AttachCurrentThread(jvm, &env, NULL);

#endif 	
	if (res < 0) {
		fprintf(stderr, "Attach failed\n");
		return;
	}	   		
#pragma convert(819)
	myClass = (*env)->FindClass(env,arg819[2]);
#pragma convert(0)
	if (! myClass) {
		printf("Failed to find class");
		return NULL;
	}

#pragma convert(819)
	mainID = (*env)->GetStaticMethodID(env,myClass,"main",
			"([Ljava/lang/String;)V");
#pragma convert(0)
	if (! mainID) {
		printf("Failed to find jmethodID of 'main'");
		return NULL;
	}

#pragma convert(819)
	stringClass = (*env)->FindClass(env,"java/lang/String");
#pragma convert(0)
	if (! stringClass) {
		printf("Failed to find java/lang/String\n");
		return NULL;
	}

	/*  Now, you need to create an empty array of strings,
	 *  since main requires such an array as a parameter.
	 */
	argstr =(*env)->NewObjectArray(env,0,stringClass,0);
	if (! argstr) {
		printf( "Failed to create args array\n");
		return (NULL);
	}

	(*env)->CallStaticVoidMethod(env,myClass,mainID,argstr);



	/* Check for errors. */
	if ((*env)->ExceptionOccurred(env)) {
		(*env)->ExceptionDescribe(env);
	}

	return NULL;
}

int main(int argc, char *argv[]) {
	JNIEnv *env1;
	int inp=0;
	unsigned int len[3];
	pthread_t thread;
	int                   rc=0;
	pthread_attr_t        pta;
	thread_parm_t         *parm=NULL;
	JavaVMInitArgs vm_args;
	JavaVMOption options[1];


	pthread_option_np_t  pthreadOptions;

	printf("Enter Testcase - %s\n", argv[0]);
	//Try create pthread env. 
	printf("Create a thread attributes object\n");
	rc = pthread_attr_init(&pta);
	checkResults("pthread_attr_init()\n", rc);


	memset(&pthreadOptions,0x00,sizeof(pthread_option_np_t));
	pthreadOptions.option = PTHREAD_OPTION_CREATE_THREAD_NP;
	pthreadOptions.optionValue = PTHREAD_OPTIONVALUE_SYSTEM_NP;
	rc = pthread_setpthreadoption_np(&pthreadOptions);
	checkResults("pthread_setpthreadoption_np()\n", rc);

#ifdef JNI_VERSION_1_2

	printf("argv[1]=%s,argv[2]=%s\n",argv[1],argv[2]);

	if(convert(0,819,argv[1],strlen(argv[1]),&arg819[1],&len[1]) || convert(0,819,argv[2],strlen(argv[2]),&arg819[2],&len[2])){
		printf("Failed to convert args\n");
		return -1;
	}
	options[0].optionString =arg819[1];
	vm_args.version = 0x00010002;
	vm_args.options = options;
	vm_args.nOptions = 1;
	vm_args.ignoreUnrecognized = JNI_FALSE;

	/* Create the Java VM */
	// Qp2EndPase();
	if (Qp2ptrsize() != 0){ printf("pase started!\n");}
	else printf("pase not started\n");
	if(argc>=4 && strcmp(argv[3],"32")==0)
		QP2SHELL2("/usr/lib/start32");
	if(argc>=4 && strcmp(argv[3],"64")==0)
		QP2SHELL2("/usr/lib/start64");

	if (Qp2ptrsize() != 0){ printf("pase started!\n");}
	else printf("pase not started\n");
	res = JNI_CreateJavaVM(&jvm, (void**)&env1, &vm_args);

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
	res = JNI_CreateJavaVM(&jvm, &env1, &vm_args);

#endif /* JNI_VERSION_1_2 */
	if(res){
		printf("Create JVM failed %d\n",res);
		return 0;
	}
	printf("Create thread using the default attributes\n");
	/* Set up multiple parameters to pass to the thread */
	parm =(thread_parm_t *) malloc(sizeof(thread_parm_t));
	parm->value = 77;
	strcpy(parm->string, "Inside secondary thread");
	rc = pthread_create(&thread, &pta, threadfunc, (void *)parm);
	checkResults("pthread_create(&pta)\n", rc);

	printf("Destroy thread attributes object\n");
	rc = pthread_attr_destroy(&pta);
	checkResults("pthread_attr_destroy()\n", rc);

	pthread_join(thread,NULL);
	if(argc<5 || strcmp(argv[4],"0")!=0){
		(*jvm)->DestroyJavaVM(jvm);
	}
	return(0);
}
