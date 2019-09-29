/*
 * threadkey.cpp
 *
 *  Created on: Jan 2, 2017
 *      Author: zhanggan
 */




#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
static pthread_key_t msg_key;

#define DEBUG 0
#define NUMTHREADS 4

typedef struct MsgSettings {
  int level;
  FILE * destination;
}MsgSettings;
//destructor
void specific_destroy(void *sp)
{
  free(sp);
}

void do_something()
{
  // get specfic data
  MsgSettings * m = (MsgSettings*)pthread_getspecific(msg_key);
  // output according to data
  if (m->level >= DEBUG)
    fprintf(m->destination, "blub %d", m->level);
  fclose(m->destination);
}

void *thread_func(void *param)
{
  printf("starting thread\n");
  //set the specfic data
  pthread_setspecific(msg_key, param);
  // do some working, we could just always pass param, but this gets annoying in larger programs
  do_something();
  //we need from the memory?
  return NULL;
}

int main(int argc, const char *argv[])
{
  // create key, register destructor
  pthread_key_create(&msg_key, specific_destroy);
  pthread_t             thread[NUMTHREADS];
  int                   i;
  for (i=0; i <NUMTHREADS; ++i) {
    // start threads and give each a own logfile and loglevel
    char file[200];
    MsgSettings * m = ( MsgSettings *)malloc(sizeof(MsgSettings));
    snprintf(file, 200, "logfile_thread%d.txt", i);
    m->level = i;
    m->destination = fopen(file, "w");
    pthread_create(&thread[i], NULL, thread_func, m);
  }
  for (i=0; i <NUMTHREADS; ++i) {
    pthread_join(thread[i], NULL);
  }

  // delete the key and all data associated with it
  pthread_key_delete(msg_key);
  return 0;
}
