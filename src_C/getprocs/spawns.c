/**************************************************************************/
/* Application creates an child process using spawn().                    */
/**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/types.h>
#include <unistd.h>

main (int argc, char *argv[])
{
   int    i, num, pid, rc, on = 1;
   /*int    spawn_fdmap[1];*/
   char  *spawn_argv[1];
   char  *spawn_envp[1];
   struct inheritance   inherit;

   /**********************************************/
   /* Initialize the spawn parameters            */
   /*                                            */
   /* The socket descriptor for the new          */
   /* connection is mapped over to descriptor 0  */
   /* in the child program.                      */
   /**********************************************/
   memset(&inherit, 0, sizeof(inherit));
   spawn_argv[0]  = NULL;
   spawn_envp[0]  = NULL;
   /*spawn_fdmap[0] = NULL;*/

   /**********************************************/
   /* Create the worker job                      */
   /**********************************************/
   printf("ParentJob:%d\n",getpid());
   pid = spawn("/QSYS.LIB/GAVIN.LIB/CHILD.PGM",
               1, NULL, &inherit,
               spawn_argv, spawn_envp);
   if (pid < 0)
   {
      perror("spawn() failed");
      exit(-1);
   }
   printf("  spawn completed successfully,pid=%d\n",pid);

}