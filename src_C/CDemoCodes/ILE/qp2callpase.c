#include <stdio.h>
#include <qp2shell2.h>
#include <qp2user.h>
#include <pthread.h>
#define JOB_CCSID 0
typedef struct {
	int threadID;
} ThreadParm;

void * newThread(void *p)
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
    QP2SHELL2("/usr/lib/start32");

    /*
     * Qp2dlopen opens the global name space (rather than
     * loading a new shared executable) when the first
     * argument is a null pointer.  Qp2dlsym locates the
     * function descriptor for the PASE for i getpid
     * subroutine (exported by shared library libc.a)
     */
    char path[27] ;
    memset(path,0,27);
    strcpy(path,"/home/zhanggan/dividzero.so");
    id = Qp2dlopen(path, QP2_RTLD_NOW, JOB_CCSID);
    //getpid_pase = Qp2dlsym(id, "getpid", JOB_CCSID, NULL);
    getpid_pase = Qp2dlsym(id, "dividzero", JOB_CCSID, NULL);

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
    printf("PASE for i dividizero() = %i\n", result);
    if (result == -1)
        printf("IBM i errno = %i\n", *Qp2errnop());
    /*
     * Call Qp2CallPase to run the PASE for i getpid
     * function, and print the result.  Use Qp2errnop
     * to find and print the PASE for i errno if the
     * function result was -1
     */
    rc = Qp2CallPase(getpid_pase,
                         NULL,        // no argument list
                         signature,
                         QP2_RESULT_WORD,
                         &result);
    printf("PASE for i dividizero() = %i\n", result);
    if (result == -1)
        printf("IBM i errno = %i\n", *Qp2errnop());

    /*
     *  Close the Qp2dlopen instance, and then call
     *  Qp2EndPase to end PASE for i in this job
     */
    Qp2dlclose(id);
    Qp2EndPase();
    return 0;
}
int main(){
	pthread_t Thd;
	pthread_attr_t pta;
	ThreadParm tp;
	tp.threadID=8278;

	int r=pthread_attr_init(&pta);
	printf("pthread_attr_init=%d\n",r);
	int rc = 11;
	rc = pthread_create(&Thd, NULL, newThread, NULL);
	pthread_join(Thd,NULL);
	printf("exit main\n");
}
