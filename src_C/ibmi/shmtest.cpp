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
#include <errno.h>
#include <sys/shm.h>

#define SHMKEYPATH "/dev/null"  /* Path used on ftok for shmget key  */
#define SHMKEYID 0              /* Id used on ftok for shmget key    */

//#define NUMSEMS 2
#define SIZEOFSHMSEG 50

int main(int argc, char *argv[]) {

    union {
        void *shm_address;
        char chrs[sizeof(void *)];
    };
    int exist = 0;
    int shmid, rc;
    key_t shmkey;
    struct shmid_ds shmid_struct;
    char hello[] = "HelloSHM";

    shmkey = ftok(SHMKEYPATH, SHMKEYID);
    printf("zg.shmkey=%d\n", shmkey);
    if (shmkey == (key_t) -1) {
        printf("main: ftok() for shm failed\n");
        return -1;
    }

    /* Get the already created shared memory ID associated with key. */
    /* If the shared memory ID does not exist, then it will not be   */
    /* created, and an error will occur.                             */

    shmid = shmget(shmkey, SIZEOFSHMSEG, 0666 | IPC_CREAT | IPC_EXCL);
    printf("zg.shmid=%d\n", shmid);
    if (shmid == -1) {
        printf("main: shmget() failed,errno=%d\n", errno);
        if (errno == 17) { //EEXIST
            shmid = shmget(shmkey, SIZEOFSHMSEG, 0666);
            exist = 1;
//            rc = shmctl(shmid, IPC_RMID, &shmid_struct);
//            shmid = shmget(shmkey, SIZEOFSHMSEG, 0666 | IPC_CREAT | IPC_EXCL);
        } else {
            return (-1);
        }
    }

    /* Attach the shared memory segment to the client process.       */
    shm_address = shmat(shmid, NULL, 0);
    if (shm_address == NULL) {
        printf("main: shmat() failed\n");
        return -1;
    }
    printf("zg.shm_address=%p,&shm_address=%p\n", shm_address, &shm_address);
    printf("zg.shm_address=0x");
    for (int zi = sizeof(void *) - 1; zi > -1; zi--) {
        printf("%02X", chrs[zi] & 0xFF);
    }
    printf("\n");
    if (!exist) {
        memset(shm_address, 0, SIZEOFSHMSEG);
        memcpy(shm_address, hello, 8);
        printf("refresh the memory!\n");
    }
    printf("zg.hello=%s", (char *) shm_address);

    /* Detach the shared memory segment from the current process.    */
    rc = shmdt(shm_address);
    if (rc == -1) {
        printf("main: shmdt() failed\n");
        return -1;
    }
//    rc = shmctl(shmid, IPC_RMID, &shmid_struct);
//    if (rc == -1) {
//        printf("main: shmctl() failed\n");
//        return -1;
//    }
    return 0;
}

