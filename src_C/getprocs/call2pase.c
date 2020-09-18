#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <unistd.h>
/* include file for QP2RunPase(). */

#include <qp2user.h>

/******************************************
  Sample:
  A simple ILE C program to invoke an PASE
  for i program using QP2RunPase() and
  passing one string parameter.
  Example compilation:
    CRTCMOD MODULE(MYLIB/SAMPLEILE) SRCFILE(MYLIB/QCSRC)
    CRTPGM PGM(MYLIB/SAMPLEILE) 
******************************************/

void main(int argc, char*argv[])
 {
  /* Path name of PASE program */
  char *PasePath = "/qopensys/home/zhanggan/worktemp/pasehello";
  /* Return code from QP2RunPase() */
  int rc;
  /* The parameter to be passed to the
     i5/OS PASE program */
  char *PASE_parm = "My Parm";
  /* Argument list for i5/OS PASE program,
     which is a pointer to a list of pointers */
  char **arg_list;
  /* allocate the argument list */
  arg_list =(char**)malloc(3 * sizeof(*arg_list));
  /* set program name as first element. This is a UNIX convention */
  arg_list[0] = PasePath;
  /* set parameter as first element */
  arg_list[1] = PASE_parm;
  /* last element of argument list must always be null */
  arg_list[2] = 0;
  printf("Parent:%d\n",getpid());
  /* Call i5/OS PASE program. */
   rc = Qp2RunPase(PasePath, /* Path name */
      NULL,           /* Symbol for calling to ILE, not used in this sample */
      NULL,           /* Symbol data for ILE call, not used here */
      0,              /* Symbol data length for ILE call, not used here */
      819,            /* ASCII CCSID for i5/OS PASE */
      arg_list,       /* Arguments for i5/OS PASE program */
      NULL);          /* Environment variable list, not used in this sample */
 }