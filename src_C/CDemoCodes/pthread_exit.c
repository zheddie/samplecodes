/* File pthread_exit.c created by Zhang Gan,917202 on Sun May 23 2010. */

/* Change Activity: */
/* End Change Activity */

static char ibmid[] = "Copyright IBM Corporation 2010 LICENSED MATERIAL - PROGRAM PROPERTY OF IBM";

#define _MULTI_THREADED
#include <pthread.h>
#include <stdio.h>
/*#include "check.h"*/

int  theStatus=5;

void *threadfunc(void *parm)
{
  printf("Inside secondary thread\n");
  pthread_exit(__VOID(theStatus));
  return __VOID(theStatus); /* Not needed, but this makes the compiler smile */
}

int main(int argc, char **argv)
{
  pthread_t             thread;
  int                   rc=0;
  void                 *status;

  printf("Enter Testcase - %s\n", argv[0]);

  printf("Create thread using attributes that allow join\n");
  rc = pthread_create(&thread, NULL, threadfunc, NULL);
  printf("pthread_create()=%d\n", rc);

  printf("Wait for the thread to exit\n");
  rc = pthread_join(thread, &status);
  printf("pthread_join()=%d\n", rc);
  if (__INT(status) != theStatus) {
    printf("Secondary thread failed\n");
    exit(1);
  }

  printf("Got secondary thread status as expected\n");
  printf("Main completed\n");
  return 0;
}


/* Change Log
<@log@>

Sun May 23 2010  14:55:26  by Zhang Gan,917202
<reason><version><Brief description and why change was made.>

Sun May 23 2010  12:21:16  by Zhang Gan,917202
<reason><version><Brief description and why change was made.>
*/
