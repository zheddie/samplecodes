/* Name: ileMain.c
 *
 * Call an PASE for i procedure from ILE
 *
 * This example uses the Qp2dlopen, Qp2dlsym, and Qp2CallPase2 ILE
 * functions to call an PASE for i function passing in parameters
 *
 * Compile like so:
 *
 * CRTBNDC PGM(mylib/ilemain)
 *         SRCFILE(mylib/mysrcpf)
 *         TERASPACE(*YES *TSIFC)
 */
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <qp2user.h>
/* Use EBCDIC default job CCSID in Qp2dlopen and Qp2dlsym calls */
#define JOB_CCSID 0

void common_putenv ()
{
	static void *pase_putenv=NULL;
	const char inputString4Put[] = {0x5A, 0x5A,0x3D,0x5A,0x5A,0x5A, 0x00};  /*ZZ=ZZZ*/
	if (Qp2ptrsize() == 0) 
	{
		//call ile putenv 
		printf("set env in ile\n");
  		/*putenv(env);*/
	}
	else{

		QP2_ptr64_t pase_mem;
		/*QP2_ptr64_t putenv_arg ;*/
		QP2_word_t result;
		const QP2_arg_type_t putenv_Signature[] = {QP2_ARG_PTR64, QP2_ARG_END};
		int rc;
		int * paseErrno;
		    struct {
			QP2_ptr64_t iP;
		    } paseFunctionArglist;
		    /* "inputAndResultBuffer" is the buffer of storage shared between ILE and PASE
		     * by Qp2CallPase2.  This buffer contains space for the PASE function result */
		    struct {
			QP2_dword_t result;
			char inputValue[7];
		    } inputAndResultBuffer;
		printf("set env in pase\n");
		if(!pase_putenv){
			QP2_ptr64_t id;
			id = Qp2dlopen(NULL, QP2_RTLD_NOW, 0); //QOpenSys/usr/lib/libc_r.a
			if (id == 0) {
			printf("Qp2dlopen failed.  ILE errno=%i\n", errno);
			if ((paseErrno=Qp2errnop()) != NULL)
			    printf("Qp2dlopen failed.  i5/OS PASE errno=%i\n", *paseErrno);
			printf("Qp2dlopen failed.  Qp2dlerror = %s\n", Qp2dlerror());
			}

			pase_putenv = Qp2dlsym(id, "putenv", 0, NULL);
			printf("env is %p,%s\n",pase_putenv,Qp2dlerror());
			
		}

	

		printf("env is %s\n",inputString4Put);

		strcpy(inputAndResultBuffer.inputValue,inputString4Put);
		paseFunctionArglist.iP = (QP2_ptr64_t)(offsetof(inputAndResultBuffer,inputValue) | QP2_ARG_PTR_TOSTACK);
		rc = Qp2CallPase2(pase_putenv ,
				&paseFunctionArglist,
				putenv_Signature,
				QP2_RESULT_DWORD,
				&inputAndResultBuffer,
				sizeof(inputAndResultBuffer));

		printf("putenv return %d,rc=%d,%s\n",inputAndResultBuffer.result,rc,strerror(errno));
		if (rc != 0) {
    	printf("Qp2CallPase failed.  rc=%i, ILE errno=%i\n", rc, errno);
			if ((paseErrno=Qp2errnop()) != NULL)
				printf("Qp2CallPase failed.  i5/OS PASE errno=%i\n", *paseErrno);
			printf("Qp2CallPase failed.  Qp2dlerror=%s\n", Qp2dlerror());
    }


	}

}
/* start PASE for i in this process */
void startPASE(void) {
    /* start64 starts the 64 bit version of PASE for i */
    char *start64Path="/usr/lib/start64";
    char *arg_list[2];

    arg_list[0] = start64Path;
    arg_list[1] = NULL;
    Qp2RunPase(start64Path,
               NULL,
               NULL,
               0,
               819,
               (char**)&arg_list,
               NULL);

}

/* open a shared library */

QP2_ptr64_t openlib(char * libname) {
    QP2_ptr64_t id;
    int * paseErrno;

    /* Qp2dlopen dynamically loads the specified library returning an
     * id value that can be used in calls to Qp2dlsym and Qp2dlcose */
    id = Qp2dlopen(libname,
                   (QP2_RTLD_NOW |
                    QP2_RTLD_MEMBER ),
                   JOB_CCSID);
    if (id == 0) {
        printf("Qp2dlopen failed.  ILE errno=%i\n", errno);
        if ((paseErrno=Qp2errnop()) != NULL)
            printf("Qp2dlopen failed.  PASE for i errno=%i\n", *paseErrno);
        printf("Qp2dlopen failed.  Qp2dlerror = %s\n", Qp2dlerror());
    }

    return(id);
}

/* find an exported symbol */

void * findsym(const QP2_ptr64_t id, const char * functionname) {
    void * symbol;
    int * paseErrno;

    /* Qp2dlsym locates the function descriptor for the
     * specified function */
    symbol = Qp2dlsym(id, functionname, JOB_CCSID, NULL);
    if (symbol == NULL) {
        printf("Qp2dlsym failed.  ILE errno = %i\n", errno);
        if ((paseErrno=Qp2errnop()) != NULL)
            printf("Qp2dlsym failed.  PASE for i errno=%i\n", *paseErrno);
        printf("Qp2dlsym failed.  Qp2dlerror = %s\n", Qp2dlerror());
    }
    return(symbol);
}

/* call PASE for i procedure */
int callPASE(const void * functionsymbol,
             const void * arglist,
             const QP2_arg_type_t * signature,
             const QP2_result_type_t result_type,
             void * buf,
             const short buflen) {
    int * paseErrno;
    int rc;

    /* Call Qp2CallPase2 to run the unction function */
    rc = Qp2CallPase2(functionsymbol,
                      arglist,
                      signature,
                      result_type,
                      buf,
                      buflen);
    if (rc != 0) {
        printf("Qp2CallPase failed.  rc=%i, ILE errno=%i\n", rc, errno);
        if ((paseErrno=Qp2errnop()) != NULL)
            printf("Qp2CallPase failed.  PASE for i errno=%i\n", *paseErrno);
        printf("Qp2CallPase failed.  Qp2dlerror=%s\n", Qp2dlerror());
    }
}


int main(int argc, char *argv[])
{
    /* we will call a function in PASE for i named "paseFunction"
     * the prototype for the function looks like this:
     *  int paseFunction(void * input, void * output ) */

    /* "signature" is the argument signature for the PASE routine "paseFunction" */
    const QP2_arg_type_t signature[] = {QP2_ARG_PTR64, QP2_ARG_PTR64, QP2_ARG_END};

    /* "paseFunctionArglist" are the arguments for the PASE routine "paseFunction" */
    struct {
        QP2_ptr64_t inputPasePtr;
        QP2_ptr64_t outputPasePtr;
    } paseFunctionArglist;


    /* "inputString" will be one of the arguments to the PASE routine
     * "paseFunction" we will call
     * This is the string "input" in ASCII */
    
    const char inputString[] = {0x5A, 0x5A, 0x00};
    /*char * inputString = NULL;*/

    /* "outputILEPtr" will be a pointer to storage malloc'd from PASE heap */
    char * outputILEPtr;

    /* "id" is the identifier for the library opened by Qp2dlopen */
    QP2_ptr64_t id;

    /* "paseFunction_ptr" is the pointer to the routine "paseFunction" in PASE */
    void * paseFunction_ptr;

    /* "inputAndResultBuffer" is the buffer of storage shared between ILE and PASE
     * by Qp2CallPase2.  This buffer contains space for the PASE function result */
    struct {
        QP2_dword_t result;
        char inputValue[6];
    } inputAndResultBuffer;

    int rc;
    int * paseErrno;
    char ge[] = {0x5A,0x5A,0x00};
/*
#pragma convert(819)
    
    inputString =(char *) malloc(sizeof(char)*5);
    strcpy(inputString , "HOME");
#pragma convert(0)*/
    /* start PASE for i in this process */
    startPASE();
/*for put env*/

  common_putenv();


printf("getenv in ILE:%s\n",getenv(ge));
printf("getenv in ILE:ZZ=%s\n",getenv("ZZ"));
/*For get env*/
    id = openlib("/home/zhanggan/demos/libpasefn.a(shr64.o)");

    if (id !=0) {
        /* Locate the symbol for "paseFunction" */
        paseFunction_ptr = findsym(id, "zgetenv");

        if (paseFunction_ptr != NULL) {

            /* set input arguments for the call to paseFunction() */

            /* copy the inputString into the inputAndResultBuffer */
            strcpy(inputAndResultBuffer.inputValue, inputString);

            /* by setting inputPasePtr argument to the offset of the
             * inputValue by-address argument data in the
             * inputAndResultbuffer structure and OR'ing that with
             * QP2_ARG_PTR_TOSTACK QP2CallPase2 will "fixup" the
             * actual argument pointer passed to the PASE function
             * to point to the address (plus the offset) of the
             * copy of the inputAndResultbuffer that Qp2CallPase2
             * copies to PASE for i storage */
            paseFunctionArglist.inputPasePtr =
            (QP2_ptr64_t)((offsetof(inputAndResultBuffer, inputValue))
                          | QP2_ARG_PTR_TOSTACK);

            /* allocate memory from the PASE for i heap for an output
             * argument.  Qp2malloc will also set the PASE for i address
             * of the allocated storage in the outputPasePtr
             * argument */
            outputILEPtr = Qp2malloc(80, &(paseFunctionArglist.outputPasePtr));

            /* Call the function in PASE for i */
            rc = callPASE(paseFunction_ptr,
                          &paseFunctionArglist,
                          signature,
                          QP2_RESULT_DWORD,
                          &inputAndResultBuffer,
                          sizeof(inputAndResultBuffer));
            printf("output from paseFunction = >%s<\n",
                       (char*)outputILEPtr);
            printf("return code from paseFunction = %d\n",
                       (int)inputAndResultBuffer.result);
            rc = callPASE(paseFunction_ptr,
                          &paseFunctionArglist,
                          signature,
                          QP2_RESULT_DWORD,
                          &inputAndResultBuffer,
                          sizeof(inputAndResultBuffer));
            printf("output from paseFunction = >%s<\n",
                       (char*)outputILEPtr);
            printf("return code from paseFunction = %d\n",
                       (int)inputAndResultBuffer.result);
        } /* paseFunction_ptr != NULL */
    } /* id != 0 */

    /* Close the Qp2dlopen instance, and then call Qp2EndPase
     * to end PASE for i in this job */
    Qp2dlclose(id);
    Qp2EndPase();
    return 0;
}
