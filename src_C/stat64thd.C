#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pthread.h>

int do_something(){
    struct stat64 sb;
    printf("zg.QDLS.getchar\n");
    getchar();
    int rt = stat64("/QDLS", &sb);
    if(rt == 0){
        printf("zg.QDLS.mode=%d\n",sb.st_mode);
    }else{
        printf("zg.QDLS.rt=%d\n",rt);
    }
    printf("zg.qopensys.getchar\n");
    getchar();
    rt = stat64("/qopensys", &sb);
    if(rt == 0){
        printf("zg.qopensys.mode=%d\n",sb.st_mode);
    }else{
        printf("zg.qopensys.rt=%d\n",rt);
    }

	return(0);
}

void *thread_func(void *param)
{
  printf("zg.in thread_func().starting thread\n");
  //set the specfic data
  do_something();
  //we need from the memory?
  return NULL;
}
#define MAXTHREAD 1
int main(int argc, const char *argv[])
{
  // create key, register destructor
  pthread_t             thread[MAXTHREAD];
  int                   i;
  for (i=0; i <MAXTHREAD; ++i) {
    pthread_create(&thread[i], NULL, thread_func, NULL);
  }
  for (i=0; i <MAXTHREAD; ++i) {
    pthread_join(thread[i], NULL);
  }
  printf("zg.in main().CP9\n");
  return 0;
}