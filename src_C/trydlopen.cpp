#include <stdio.h>
#include <stdlib.h>
#include <qp2shell2.h>
#include <qp2user.h>
#define JOB_CCSID 0

int main(int argc, char *argv[])
{
    QP2_ptr64_t id;
    void *getpid_pase;
    const QP2_arg_type_t signature[] = { QP2_ARG_END };
    QP2_word_t result;

    /*
     * Call QP2SHELL2 to run the PASE for i program
     * /usr/lib/start32, which starts PASE for i in
     * 32-bit mode (and leaves it active on return)
     */
    QP2SHELL2("/usr/lib/start64");

    /*
     * Qp2dlopen opens the global name space (rather than
     * loading a new shared executable) when the first
     * argument is a null pointer.  Qp2dlsym locates the
     * function descriptor for the PASE for i getpid
     * subroutine (exported by shared library libc.a)
     */
    id = Qp2dlopen(NULL, QP2_RTLD_NOW, JOB_CCSID);
    printf("zg.QP2_ptr64_t ID=%x\n",id);
    if(id == 0){
        printf("zg.id == 0\n");
    }else{
        printf("zg.id != 0\n");
    }
    getpid_pase = Qp2dlsym(id, "getpid", JOB_CCSID, NULL);

    /*
     * Call Qp2CallPase to run the PASE for i getpid
     * function, and print the result.  Use Qp2errnop
     * to find and print the PASE for i errno if the
     * function result was -1
     */
    int rc = Qp2CallPase(getpid_pase,
                         NULL,        // no argument list
                         signature,
                         QP2_RESULT_WORD,
                         &result);
    printf("PASE for i getpid() = %i\n", result);
    if (result == -1)
        printf("IBM i errno = %i\n", *Qp2errnop());

    /*
     *  Close the Qp2dlopen instance, and then call
     *  Qp2EndPase to end PASE for i in this job
     */
    Qp2dlclose(id);
    QP2_ptr64_t paseJniLibraryId = -1;
   char* dlError;
    char PaseJniInterface[] = "libi5osenv.so";
    char buffer[512];
    char * penv = NULL;
    char* interface = PaseJniInterface;
    printf("zg.openSharedLibrary: %s\n", interface);
    // Note, a call to Qp2dlopen is not thread safe.  The 1st call
    // to this routine is done from the Qxj9Vm constructor to ensure
    // only one thread is using this routine. 
    //setenv("LIBPATH","/QOpenSys/QIBM/ProdData/JavaVM/jdk80/64bit/jre/lib/ppc64/",1);
    putenv("LIBPATH=/QOpenSys/QIBM/ProdData/JavaVM/jdk80/64bit/jre/lib/ppc64/");
    penv = getenv("LIBPATH");
    printf("zg.libpath=%s\n",penv);
    paseJniLibraryId = Qp2dlopen(interface,
                 QP2_RTLD_NOW,
                 JOB_CCSID);
    // Was the call to dlopen successful?  
    printf("zg.openSharedLibrary,paseJniLibraryId=%x\n",paseJniLibraryId);
    if(paseJniLibraryId == 0){
        printf("zg.openSharedLibrary,paseJniLibraryId==0\n");
        // dlError = Qp2dlerror();                             // @A5M
        // printf("zg.openSharedLibrary FAILED: %s\n", dlError); // @A5C}
    }else{
        printf("zg.openSharedLibrary,paseJniLibraryId!=0\n");
    }
    printf("zg.openSharedLibrary,cp9\n");
    Qp2EndPase();
    return 0;
}