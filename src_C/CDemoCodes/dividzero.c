#define _MULTI_THREADED
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
//#include "check.h"

#define PTHREAD_OPTION_CREATE_THREAD_NP  0x40C3D9E3  // " CRT" in hex
#define PTHREAD_OPTIONVALUE_SYSTEM_NP    0x40E2E8E2  // " SYS" in hex
#define PTHREAD_OPTIONVALUE_USER_NP      0x00000000  // User Thread

typedef struct {
  int   value;
  char  string[128];
} thread_parm_t;

void * checkResults(char * str,int val){
	printf("rt=%d,str=%s",val,str);
	return NULL;
}


void *threadfunc(void *parm)
{
  float test = 10.0;
 float zero = 0.0;
  thread_parm_t *p = (thread_parm_t *)parm;
  printf("%s, parm = %d\n", p->string, p->value);
  test = test/zero;
  printf("zg.test=%f\n",test);
  free(p);
  return NULL;
}

int main(int argc, char **argv)
{
  pthread_t             thread;
  int                   rc=0;
  pthread_attr_t        pta;
  thread_parm_t         *parm=NULL;


  pthread_option_np_t  pthreadOptions;

  printf("Enter Testcase - %s\n", argv[0]);

  printf("Create a thread attributes object\n");
  rc = pthread_attr_init(&pta);
  checkResults("pthread_attr_init()\n", rc);


	memset(&pthreadOptions,0x00,sizeof(pthread_option_np_t));
	pthreadOptions.option = PTHREAD_OPTION_CREATE_THREAD_NP;
	pthreadOptions.optionValue = PTHREAD_OPTIONVALUE_SYSTEM_NP;
	rc = pthread_setpthreadoption_np(&pthreadOptions);
	  checkResults("pthread_setpthreadoption_np()\n", rc);

  /* Create 2 threads using default attributes in different ways */
  printf("Create thread using the NULL attributes\n");
  /* Set up multiple parameters to pass to the thread */
  parm = (thread_parm_t *)malloc(sizeof(thread_parm_t));
  parm->value = 5;
  strcpy(parm->string, "Inside secondary thread");
  rc = pthread_create(&thread, NULL, threadfunc, (void *)parm);
  checkResults("pthread_create(NULL)\n", rc);

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

  /* sleep() is not a very robust way to wait for the thread */
  sleep(5);

  printf("Main completed\n");
  return 0;
}
