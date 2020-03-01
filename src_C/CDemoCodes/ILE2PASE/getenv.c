#include <qp2user.h>
#include <stdlib.h>
#include <stdio.h>
#include <qp2shell2.h>
#include <errno.h>

#define JOB_CCSID 0

int main(int argc, char *argv[])
{
    QP2_ptr64_t id;
    void *getpid_pase,*getenv_pase,*putenv_pase;
    const QP2_arg_type_t signature[] = {QP2_ARG_PTR32, QP2_ARG_END };
    QP2_ptr64_t ptr64,ptr64rt;
    struct {
        QP2_ptr32_t inputPasePtr;
    } paseFunctionArglist;
    struct {
        QP2_ptr32_t outputPasePtr;
    } paseFunctionOutput;
    QP2_ptr32_t result;
    int rc = 0;
   char * ILEptr ,*ILEptrRT;
    /*
     * Call QP2SHELL2 to run the i5/OS PASE program
     * /usr/lib/start32, which starts i5/OS PASE in
     * 32-bit mode (and leaves it active on return)
     */
    QP2SHELL2("/usr/lib/start32");

    /*
     * Qp2dlopen opens the global name space (rather than
     * loading a new shared executable) when the first
     * argument is a null pointer.  Qp2dlsym locates the
     * function descriptor for the i5/OS PASE getpid
     * subroutine (exported by shared library libc.a)
     */
    id = Qp2dlopen(NULL, QP2_RTLD_NOW, JOB_CCSID);
    getenv_pase = Qp2dlsym(id, "getenv", JOB_CCSID, NULL);

    /*
     * Call Qp2CallPase to run the i5/OS PASE getpid
     * function, and print the result.  Use Qp2errnop
     * to find and print the i5/OS PASE errno if the
     * function result was -1
     */
    ILEptr = Qp2malloc(10,&ptr64);
    /*memcpy(ILEptr,0,sizeof(ILEptr));*/
#pragma convert(819)
    strcpy(ILEptr,"ZGENV");

#pragma convert(0)
    putenv("ZGENV=HELLOENV");
    printf("getenv=%s\n",getenv("ZGENV"));
    paseFunctionArglist.inputPasePtr = (QP2_ptr32_t)ptr64;
    /*ILEptrRT = Qp2malloc(250,&ptr64rt);
    strcpy(ILEptrRT,"Can you see me?\n");
		printf("Try = %s\n", ILEptrRT);
    result = (QP2_ptr32_t)ptr64rt;
    memcpy(ILEptrRT,0,sizeof(ILEptrRT));*/
    rc = Qp2CallPase(getenv_pase,
                         &paseFunctionArglist,        // no argument list
                         signature,
                         QP2_RESULT_PTR32,
			 &result);
    		printf("getenv return %d\n",rc);
		printf("result = %s\n", (char *)result);
    if (rc != 0)
        printf("i5/OS errno = %i\n", *Qp2errnop());

    /*
     *  Close the Qp2dlopen instance, and then call
     *  Qp2EndPase to end i5/OS PASE in this job
     */
    Qp2dlclose(id);
    Qp2EndPase();
    return 0;
}
