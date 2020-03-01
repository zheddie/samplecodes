/* Name:  os400proc.c
 *
 * Call ILE procedures from PASE
 *
 * This example uses the PASE _ILECALL function to call
 * a function in an ILE *SRVPGM
 *
 * Compile with something like:
 *   xlc -qalign=natural -qldbl128 -o os400proc os400proc.c
 *
 * The compiler options -qalign=natural and -qldbl128 are
 * necessary only when interacting with OS/400 ILE programs
 * to force relative 16-byte alignment of type long double
 * (used inside type ILEpointer)
 *
 */

#include <pthread.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <stdio.h>
#include <as400_types.h>
#include <as400_protos.h>

/*
 * init_pid saves the process id (PID) of the process that
 * extracted the ILEpointer addressed by ILEtarget.
 * init_pid is initialized to a value that is not a
 * valid PID to force initialization on the first
 * reference after the exec() of this program
 */

pid_t  init_pid = -1;
ILEpointer* ileProcedure;  /* pointer to ILE routine ileProcedure */
ILEpointer* cleanup;  /* pointer to ILE routine cleanup */

/*
 * ROUND_QUAD finds a 16-byte aligned memory
 * location at or beyond a specified address
 */

#define ROUND_QUAD(x) (((size_t)(x) + 0xf) & ~0xf)

/*
 * do_init loads an ILE service program and extracts an
 * ILE pointer to a procedure that is exported by that
 * service program.
 */

void do_init()
{
    static char ileProcedure_buf[sizeof(ILEpointer) + 15];
    static char cleanup_buf[sizeof(ILEpointer) + 15];
    long long int actmark;
    int rc;

    actmark = _ILELOADX("GAVIN/ILEPASE", ILELOAD_LIBOBJ);
    if (actmark == -1)
        abort();

    /*
     * xlc does not guarantee 16-byte alignment for
     * static variables of any type, so we find an
     * aligned area in an oversized buffer. _ILESYM()
     * extracts an ILE procedure pointer from the
     * service program activation
     */

    ileProcedure = (ILEpointer*)ROUND_QUAD(ileProcedure_buf);
    rc = _ILESYMX(ileProcedure, actmark, "ileProcedure");
    if (rc == -1)
        abort();

    cleanup = (ILEpointer*)ROUND_QUAD(cleanup_buf);
    rc = _ILESYMX(cleanup, actmark, "cleanup");
    if (rc == -1)
        abort();


    /*
     * Save the current PID in static storage so we
     * can determine when to re-initialize (after fork)
     */
    init_pid = getpid();
}


int ileProcedure_pase(int arg1)
{
    static result_type_t result_type = RESULT_INT32;
    static arg_type_t signature[] =
    {
        ARG_INT32,
        ARG_END
    };
    typedef struct {
        ILEarglist_base base;
        int32 arg1;
    } ILEarglistSt;
    /*
     * xlc does not guarantee 16-byte alignment for
     * automatic (stack) variables of any type, so we
     * find an aligned area in an oversized buffer
     */
    char ILEarglist_buf[sizeof(ILEarglistSt) + 15];
    ILEarglistSt *ILEarglist = (ILEarglistSt*)ROUND_QUAD(ILEarglist_buf);
    /*
     * Assignment statements are faster than calling
     * build_ILEarglist()
     */
    ILEarglist->arg1 = arg1;
    /*
     * Use a saved PID value to check if the ILE pointer
     * is set. ILE procedure pointers inherited by the
     * child process of a fork() are not usable because
     * they point to an ILE activation group in the parent
     * process
     */
    if (getpid() != init_pid)
        do_init();
    printf("Before ileProcedure call to ILE service program!\n");
    /*
     * _ILECALL calls the ILE procedure. The stack may
     * be unwound, but no heap storage is orphaned if
     * an exception or signal occurs
     */
    _ILECALL(ileProcedure,
             &ILEarglist->base,
             signature,
             result_type);
    return ILEarglist->base.result.s_int32.r_int32;
}

void cleanup_pase(void * arg1)
{
    static result_type_t result_type = RESULT_VOID;
    static arg_type_t signature[] =
    {
        ARG_MEMPTR,
        ARG_END
    };
    typedef struct {
        ILEarglist_base base;
        ILEpointer arg1;
    } ILEarglistSt;
    /*
     * xlc does not guarantee 16-byte alignment for
     * automatic (stack) variables of any type, so we
     * find an aligned area in an oversized buffer
     */
    char ILEarglist_buf[sizeof(ILEarglistSt) + 15];
    ILEarglistSt *ILEarglist = (ILEarglistSt*)ROUND_QUAD(ILEarglist_buf);
    /*
     * Use a saved PID value to check if the ILE pointer
     * is set. ILE procedure pointers inherited by the
     * child process of a fork() are not usable because
     * they point to an ILE activation group in the parent
     * process
     */
    if (getpid() != init_pid)
        do_init();
    /*
     * _ILECALL calls the ILE procedure. The stack may
     * be unwound, but no heap storage is orphaned if
     * an exception or signal occurs
     */
    _ILECALL(cleanup,
             &ILEarglist->base,
             signature,
             result_type);
    return;
}


/* Simple function to check the return code and exit the program
   if the function call failed
   */
static void checkResults(char *string, int rc) {
    if (rc) {
        printf("Error on : %s, rc=%d",
               string, rc);
        exit(EXIT_FAILURE);
    }
    return;
}

void pasecleanup(void * junk)
{
    printf("In pasecleanup - pthread_exiting\n");
    cleanup_pase(junk);

}


void *threadfunc(void *parm)
{
    int            rc;
    void * pushparm = NULL;

    pthread_t     self = pthread_self();

    printf("Enter Secondary thread, tid=%.8x pid=%.8x [%d]\n",
           self, getpid(), getpid());

    pthread_cleanup_push(&pasecleanup, &pushparm);

    rc = ileProcedure_pase(12);
    printf("wrapper_2 rc=%d\n", rc);

    pthread_exit(NULL);
}


int main (int argc, char* argv[]) {
    pthread_t thread;
    int rc;
    void                  *status=NULL;


    printf("Create thread using the NULL attributes\n");
    rc = pthread_create(&thread, NULL, threadfunc, NULL);
    checkResults("pthread_create(NULL)\n", rc);

    rc = pthread_join(thread, &status);
    checkResults("pthread_join()\n", rc);
    if (((int)status) != 0) {
        printf("Secondary thread failed\n");
        exit(1);
    }

    printf("Main completed\n");
    return 0;
}
