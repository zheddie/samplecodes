#include <qp2user.h>
#include <stdlib.h>
#include <stdio.h>
#include <qp2shell2.h>
#include <errno.h>

#define JOB_CCSID 0

int main(int argc, char *argv[])
{
    QP2_ptr64_t id;
    void *getpid_pase;
    const QP2_arg_type_t signature[] = { QP2_ARG_END };
    QP2_word_t result;
    int rc = 0;

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
    getpid_pase = Qp2dlsym(id, "getpid", JOB_CCSID, NULL);

    /*
     * Call Qp2CallPase to run the i5/OS PASE getpid
     * function, and print the result.  Use Qp2errnop
     * to find and print the i5/OS PASE errno if the
     * function result was -1
     */
    rc = Qp2CallPase(getpid_pase,
                         NULL,        // no argument list
                         signature,
                         QP2_RESULT_WORD,
                         &result);
    printf("i5/OS PASE getpid() = %i\n",result);
    if (result == -1)
        printf("i5/OS errno = %i\n", *Qp2errnop());

    /*
     *  Close the Qp2dlopen instance, and then call
     *  Qp2EndPase to end i5/OS PASE in this job
     */
    Qp2dlclose(id);
    Qp2EndPase();
    return 0;
}
