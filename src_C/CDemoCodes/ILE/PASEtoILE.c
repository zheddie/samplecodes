/* Name: PASEtoILE.c  
 *  
 * You must use compiler options -qalign=natural and -qldbl128  
 * to force relative 16-byte alignment of type long double  
 * (used inside type ILEpointer)  
 *
 */
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/types.h>
#include <stdio.h>
#include "as400_types.h"
#include "as400_protos.h"
 
/*
 * init_pid saves the process id (PID) of the process that
 * extracted the ILEpointer addressed by ILEtarget.
 * init_pid is initialized to a value that is not a
 * valid PID to force initialization on the first
 * reference after the exec() of this program
 *
 * If your code uses pthread interfaces, you can
 * alternatively provide a handler registered using
 * pthread_atfork() to re-initialize ILE procedure
 * pointers in the child process and use a pointer or
 * flag in static storage to force reinitialization
 * after exec()
 */
 
pid_t  init_pid = -1;
ILEpointer*ILEtarget;  /* pointer to ILE procedure */
ILEpointer * ILEdividzero;
/*
 * ROUND_QUAD finds a 16-byte aligned memory
 * location at or beyond a specified address
 */
 
#define ROUND_QUAD(x) (((size_t)(x) + 0xf) & ~0xf)
 
/*
 * do_init loads an ILE service program and extracts an
 * ILEpointer to a procedure that is exported by that
 * service program.
 */
 
void do_init()
{
   static char ILEtarget_buf[sizeof(ILEpointer) + 15];
   static char ILEdividzero_buf[sizeof(ILEpointer) + 15];

   unsigned long long actmark;
   int rc;
 
   /* _ILELOADX() loads the service program */
   actmark = _ILELOADX("GAVIN/MYSRVPGM", ILELOAD_LIBOBJ);
   if (actmark == -1)
     abort();
 
   /*
    * xlc does not guarantee 16-byte alignment for
    * static variables of any type, so we find an
    * aligned area in an oversized buffer. _ILESYMX()
    * extracts an ILE procedure pointer from the
    * service program activation
    */
 
   ILEtarget = (ILEpointer*)ROUND_QUAD(ILEtarget_buf);
   rc = _ILESYMX(ILEtarget, actmark, "ileProcedure");
   if (rc == -1)
     abort();
   ILEdividzero = (ILEpointer*)ROUND_QUAD(ILEdividzero_buf);
   rc = _ILESYMX(ILEdividzero, actmark, "dividzero");
   if (rc == -1)
     abort(); 
   /*
    * Save the current PID in static storage so we
    * can determine when to re-initialize (after fork)
    */
   init_pid = getpid();
}
 
/*
 * "aggregate" is an example of a structure or union
 * data type that is passed as a by-value argument.
 */
typedef struct {
    char        filler[5];
} aggregate;
 
/*
 * "result_type" and "signature" define the function
 * result type and the sequence and type of all
 * arguments needed for the ILE procedure identified
 * by ILEtarget
 *
 * NOTE: The fact that this argument list contains
 * fixed-point arguments shorter than 4 bytes or
 * floating-point arguments shorter than 8 bytes
 * implies that the target ILE C procedure is compiled
 * with #pragma argument(ileProcedureName, nowiden)
 *
 * Without this pragma, standard C linkage for ILE
 * requires 1-byte and 2-byte integer arguments to be
 * widened to 4-bytes and requires 4-byte floating-point
 * arguments to be widened to 8-bytes
 */
static result_type_t result_type = RESULT_INT32;
static arg_type_t signature[] =
{
    ARG_INT32,
    ARG_MEMPTR,
    ARG_INT64,
    ARG_UINT8,      /* requires #pragma nowiden in ILE code */
    sizeof(aggregate),
    ARG_INT16,
    ARG_END
};
 
/*
 * simple_wrapper accepts the same arguments and returns
 * the same result as the ILE procedure it calls. This
 * example does not require a customized or declared structure
 * for the ILE argument list. This wrapper uses malloc
 * to obtain storage. If an exception or signal occurs,
 * the storage may not be freed. If your program needs 
 * to prevent such a storage leak, a signal handler 
 * must be built to handle it, or you can use the methods
 * in best_wrapper.
 */
int simple_wrapper(int arg1, void *arg2, long arg3,
                   char arg4, aggregate arg5, short arg6)
{
    int result;
    /*
     * xlc does not guarantee 16-byte alignment for
     * automatic (stack) variables of any type, but
     * malloc() always returns 16-byte aligned storage.
     * size_ILEarglist() determines how much storage is
     * needed, based on entries in the signature array
     */
    ILEarglist_base *ILEarglist;
    ILEarglist = (ILEarglist_base*)malloc( size_ILEarglist(signature) );
 
    /*
     * build_ILEarglist() copies argument values into the ILE
     * argument list buffer, based on entries in the signature
     * array.
     */
    build_ILEarglist(ILEarglist, &arg1, signature);
 
    /*
     * Use a saved PID value to check if the ILEpointer
     * is set. ILE procedure pointers inherited by the
     * child process of a fork() are not usable because
     * they point to an ILE activation group in the parent
     * process
     */
    if (getpid() != init_pid)
      do_init();
 
    /*
     * _ILECALL calls the ILE procedure. If an exception or signal
     * occurs, the heap allocation is orphaned (storage leak)
     */
    _ILECALL(ILEtarget, ILEarglist, signature, result_type);
    result = ILEarglist->result.s_int32.r_int32;
    if (result == 1) {
      printf("The results of the simple wrapper is: %s\n", (char *)arg2);
    }
    else if (result == 0) 
      printf("ILE received other than 1 or 2 for version.\n");
    else 
      printf("The db file never opened.\n");
    free(ILEarglist);
    return result;
}
 
/*
 * ILEarglistSt defines the structure of the ILE argument list.
 * xlc provides 16-byte (relative) alignment of ILEpointer
 * member fields because ILEpointer contains a 128-bit long
 * double member. Explicit pad fields are only needed in
 * front of structure and union types that do not naturally
 * fall on ILE-mandated boundaries
 */
typedef struct {
    ILEarglist_base base;
    int32 arg1;
    /* implicit 12-byte pad provided by compiler */
    ILEpointer arg2;
    long arg3;
    uint8 arg4;
    char filler[7]; /* pad to 8-byte alignment */
    aggregate arg5; /* 5-byte aggregate (8-byte align) */
    /* implicit 1-byte pad provided by compiler */
    int16 arg6;
} ILEarglistSt;
 
/*
 * best_wrapper accepts the same arguments and returns
 * the same result as the ILE procedure it calls. This
 * method uses a customized or declared structure for the
 * ILE argument list to improve execution efficiency and
 * avoid heap storage leaks if an exception or signal occurs
 */
int best_wrapper(int arg1, void *arg2, long arg3,
                 char arg4, aggregate arg5, short arg6)
{
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
    ILEarglist->arg2.s.addr = (address64_t)arg2;
    ILEarglist->arg3 = arg3;
    ILEarglist->arg4 = arg4;
    ILEarglist->arg5 = arg5;
    ILEarglist->arg6 = arg6;
    /*
     * Use a saved PID value to check if the ILEpointer
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
    printf("before ILECALL\n");

    sleep(20);
        _ILECALL(ILEdividzero, &ILEarglist->base, signature, result_type);
      printf("after ILECALL\n");
      sleep(20);
    if (ILEarglist->base.result.s_int32.r_int32 == 1)
      printf("The results of best_wrapper function is: %s\n", arg2);
    else if ( ILEarglist->base.result.s_int32.r_int32 == 0)
      printf("ILE received other than 1 or 2 for version.\n");
    else 
      printf("The db file never opened.\n");
    return ILEarglist->base.result.s_int32.r_int32;
    }

void main () {
	printf("PASE entry\n");
	sleep(60);
      int version, result2;
      char dbText[ 25 ];
      long dblNumber = 5;
      char justChar = 'a';
      short shrtNumber = 3;
      aggregate agg;
      strcpy( dbText, "none" );
      for (version =2; version <=  2; version++) 
      {
        if (version == 1) { 
          result2= simple_wrapper(version, dbText, dblNumber, justChar, agg, shrtNumber); 
        } else { 
          result2= best_wrapper(version, dbText, dblNumber, justChar, agg, shrtNumber); 
        } 
      } 

      double rr = 1.0;
      double zero=0;
      rr =rr/zero;
      printf("zg.rr=%f\n",rr);
      printf("before return from PASE\n");

      sleep(20);
}
