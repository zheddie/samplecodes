#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>


sem_t* create_semaphore(int val) {
#ifdef NAMED_SEMAPHORE
    char name[254];
    snprintf(name, sizeof(name), "/mp%ld-%lu", (long)getpid(), 1567491499l);
    printf("sem name=%s\n", name);
    sem_t* ret = sem_open(name, O_CREAT | O_EXCL, 0666, val);
    if(ret == SEM_FAILED) {
        printf("*** Failed to create semaphore. Errno=%d\n", errno);
        exit(-1);
    }
    return ret;
#else
    sem_t* ret = malloc(sizeof(sem_t));
    if(sem_init(ret, 1, val)) 
        return 0;
    return ret;
#endif
}

int main(int argc, char** argv) {
#ifdef NAMED_SEMAPHORE
    printf("\nNamed semaphore test\n\n"); 
#else
    printf("\nUnnamed semaphore test\n\n");
#endif
    sem_t* semaphore = create_semaphore(0);
    printf("got semaphore at %p\n", semaphore);
    
    
    int pid = fork();
    if(pid == 0) {
        // child process
        printf("Child. Waiting on semaphore:%p\n",semaphore);
        int res = sem_wait(semaphore);
        printf("Child. Wait done\n");
        if (res < 0) {
            printf("******** Child Failed. errno=%d\n", errno);
            if (errno == EINVAL) {
                printf("Child. errno is EINVAL\n");
            }
            exit(-1);
        }
        printf("Child. Success!!!\n");
    } else {
        printf("Parent. Delaying before posting to semaphore...\n");
        sleep(5);
        printf("Parent. Checking to make sure child is still waiting...\n");
        int childStatus;
        if (0 == waitpid(pid, &childStatus, WNOHANG)) {
            printf("Parent. Looks like the child is still alive...\n");
        } else {
            printf("***** Parent detected failure. Looks like the child died already. \n");
            exit(-2);
        } 
        
        printf("Parent. Posting to semaphore...\n");
        sem_post(semaphore);
        printf("Parent. Waiting for child processes......\n");
        while (pid = waitpid (-1, NULL, 0)){
            if (errno == ECHILD)
                break;
        }
        printf("Parent done waiting\n\n");
    }
    return 0;
}