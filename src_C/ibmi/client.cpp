/*
 * client.cpp
 *
 *  Created on: Jan 3, 2017
 *      Author: zhanggan
 */

/*********************************************************************/
/*********************************************************************/
/*                                                                   */
/* FUNCTION:  This program acts as a client to the server program.   */
/*                                                                   */
/* LANGUAGE:  ILE C                                                  */
/*                                                                   */
/* APIs USED: semget(), semop(),                                     */
/*            shmget(), shmat(), shmdt()                             */
/*            ftok()                                                 */
/*                                                                   */
/*********************************************************************/
/*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define SEMKEYPATH "/dev/null"  /* Path used on ftok for semget key  */
#define SEMKEYID 3              /* Id used on ftok for semget key    */
#define SHMKEYPATH "/dev/null"  /* Path used on ftok for shmget key  */
#define SHMKEYID 3              /* Id used on ftok for shmget key    */

#define NUMSEMS 2
#define SIZEOFSHMSEG 50

int main(int argc, char *argv[])
{
    struct sembuf operations[2];

    union {
    		void *shm_address;
    		char chrs[sizeof(void *)] ;
    };
    int semid, shmid, rc;
    key_t semkey, shmkey;

    /* Generate an IPC key for the semaphore set and the shared      */
    /* memory segment.  Typically, an application specific path and  */
    /* id would be used to generate the IPC key.                     */
    semkey = ftok(SEMKEYPATH,SEMKEYID);
    printf("zg.semkey=%d\n",semkey);
    if ( semkey == (key_t)-1 )
      {
        printf("main: ftok() for sem failed\n");
        return -1;
      }
    shmkey = ftok(SHMKEYPATH,SHMKEYID);
    printf("zg.shmkey=%d\n",shmkey);
    if ( shmkey == (key_t)-1 )
      {
        printf("main: ftok() for shm failed\n");
        return -1;
      }

    /* Get the already created semaphore ID associated with key.     */
    /* If the semaphore set does not exist, then it will not be      */
    /* created, and an error will occur.                             */
    semid = semget( semkey, NUMSEMS, 0666);
    printf("zg.semid=%d\n",semid);
    if ( semid == -1 )
      {
        printf("main: semget() failed\n");
        return -1;
      }

    /* Get the already created shared memory ID associated with key. */
    /* If the shared memory ID does not exist, then it will not be   */
    /* created, and an error will occur.                             */

    shmid = shmget(shmkey, SIZEOFSHMSEG, 0666);
    printf("zg.shmid=%d\n",shmid);
    if (shmid == -1)
      {
        printf("main: shmget() failed\n");
        return -1;
      }

    /* Attach the shared memory segment to the client process.       */
    shm_address = shmat(shmid, NULL, 0);
    printf("zg.shm_address=%p,&shm_address=%p\n",shm_address,&shm_address);
    printf("zg.shm_address=0x");
    for(int zi  = sizeof(void *)-1;zi>-1;zi--){
    		printf("%02X",chrs[zi] & 0xFF);
    }
    printf("\n");
    if ( shm_address==NULL )
      {
        printf("main: shmat() failed\n");
        return -1;
      }

    /* First, check to see if the first semaphore is a zero.  If it  */
    /* is not, it is busy right now.  The semop() command will wait  */
    /* for the semaphore to reach zero before running the semop().   */
    /* When it is zero, increment the first semaphore to show that   */
    /* the shared memory segment is busy.                            */
    operations[0].sem_num = 0;
                                    /* Operate on the first sem      */
    operations[0].sem_op =  0;
                                    /* Wait for the value to be=0    */
    operations[0].sem_flg = 0;
                                    /* Allow a wait to occur         */

    operations[1].sem_num = 0;
                                    /* Operate on the first sem      */
    operations[1].sem_op =  1;
                                    /* Increment the semval by one   */
    operations[1].sem_flg = 0;
                                    /* Allow a wait to occur         */

    rc = semop( semid, operations, 2 );
    if (rc == -1)
      {
        printf("main: semop() failed\n");
        return -1;
      }

    strcpy((char *) shm_address, "Hello from Client");

    /* Release the shared memory segment by decrementing the in-use  */
    /* semaphore (the first one).  Increment the second semaphore to */
    /* show that the client is finished with it.                     */
    operations[0].sem_num = 0;
                                    /* Operate on the first sem      */
    operations[0].sem_op =  -1;
                                    /* Decrement the semval by one   */
    operations[0].sem_flg = 0;
                                    /* Allow a wait to occur         */

    operations[1].sem_num = 1;
                                    /* Operate on the second sem     */
    operations[1].sem_op =  1;
                                    /* Increment the semval by one   */
    operations[1].sem_flg = 0;
                                    /* Allow a wait to occur         */

    rc = semop( semid, operations, 2 );
    if (rc == -1)
      {
        printf("main: semop() failed\n");
        return -1;
      }

    /* Detach the shared memory segment from the current process.    */
    rc = shmdt(shm_address);
    if (rc==-1)
      {
        printf("main: shmdt() failed\n");
        return -1;
      }

return 0;
}


