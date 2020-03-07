#include <stdio.h>
#include <sys/procfs.h>
#include <dirent.h>
#include <sys/types.h>
#include <procinfo.h>
#define BUFFSIZE 100
static int getPsinfo(pid_t pid, psinfo_t *psinfo) {
    FILE* fp;
    char fn[32];
    int ret;

    /*
     * Try to open /proc/%d/psinfo
     */

    snprintf(fn, sizeof fn, "/proc/%d/psinfo", pid);
    fp = fopen(fn, "r");
    if (fp == NULL) {
        return -1;
    }

    ret = fread(psinfo, 1, sizeof(psinfo_t), fp);
    fclose(fp);
    if (ret < sizeof(psinfo_t)) {
        return -1;
    }

    return 0;
}

pid_t unix_getParentPidAndTimings(pid_t pid,
                                  long *totalTime, long* startTime) {
    psinfo_t psinfo;
#ifndef _PASE
    if (getPsinfo(pid, &psinfo) < 0) {
        return -1;
    }

    // Validate the pid before returning the info
    if (kill(pid, 0) < 0) {
        return -1;
    }
    printf("psinfo:pid:%d,ppid:%d,uid:%d,fname:%s,psargs:%s\n",psinfo.pr_pid,psinfo.pr_ppid,psinfo.pr_uid,psinfo.pr_fname,psinfo.pr_psargs);
    *totalTime = psinfo.pr_time.tv_sec * 1000000000L + psinfo.pr_time.tv_nsec;

    *startTime = psinfo.pr_start.tv_sec * (long)1000 +
                 psinfo.pr_start.tv_nsec / 1000000;
    return (pid_t) psinfo.pr_ppid;

#else
    struct procentry64 procBuf[1];
    int rCount=0,idx=0;
    pid_t *pidx = (pid_t *)malloc(sizeof(pid_t));
    *pidx = pid;
    do{
        rCount = getprocs64(procBuf,sizeof(struct procentry64),NULL,0,pidx,1);
        //printf("rCount=%d,errno=%d\n",rCount,errno);
        for(idx=0;idx<rCount;idx++ ){
            
            if(procBuf[idx].pi_pid == pid){
                printf("pid:%d,ppid:%d,stime:%ld,state:%d,utime:%ld,stime:%ld\n",procBuf[idx].pi_pid,procBuf[idx].pi_ppid,procBuf[idx].pi_start,procBuf[idx].pi_state,procBuf[idx].pi_utime,procBuf[idx].pi_stime);
                *startTime = procBuf[idx].pi_start * 1000;
                *totalTime = procBuf[idx].pi_chk_utime ; /* set this filed based on description, but looks like we don't fill it even on AIX. */
                return procBuf[idx].pi_ppid;
            }
        }
    }while(rCount > 0);
#endif
    
}

int unix_getChildren( pid_t pid, long jarray[],
                      long jparentArray[], long jstimesArray[],int arraySize) {
    
    
    long* pids = NULL;
    long* ppids = NULL;
    long* stimes = NULL;
    int count = 0;

#ifndef _PASE
    DIR* dir;
    struct dirent* ptr;
    /*
     * To locate the children we scan /proc looking for files that have a
     * position integer as a filename.
     */

    if ((dir = opendir("/proc")) == NULL) {
        printf("Unable to open /proc\n");
        return -1;
    }

    do { // Block to break out of on Exception
        pids = jarray;
        if (pids == NULL) {
            break;
        }
        if (jparentArray != NULL) {
            ppids  = jparentArray;
            if (ppids == NULL) {
                break;
            }
        }
        if (jstimesArray != NULL) {
            stimes  = jstimesArray;
            if (stimes == NULL) {
                break;
            }
        }

        while ((ptr = readdir(dir)) != NULL) {
            pid_t ppid = 0;
            long totalTime = 0L;
            long startTime = 0L;

            /* skip files that aren't numbers */
            pid_t childpid = (pid_t) atoi(ptr->d_name);
            if ((int) childpid <= 0) {
                continue;
            }
            //printf("/proc/%d\n",childpid);
            // Get the parent pid, and start time
            ppid = unix_getParentPidAndTimings(childpid, &totalTime, &startTime);
            
            if (ppid >= 0 && (pid == 0 || ppid == pid)) {
                printf("pid:%d,totaltime:%ld,starttime:%ld\n",childpid,totalTime,startTime);
                if (count < arraySize) {
                    // Only store if it fits
                    pids[count] = (long) childpid;

                    if (ppids != NULL) {
                        // Store the parentPid
                        ppids[count] = (long) ppid;
                    }
                    if (stimes != NULL) {
                        // Store the process start time
                        stimes[count] = startTime;
                    }
                }
                count++; // Count to tabulate size needed
            }
        }
    } while (0);
    closedir(dir);
#else
    struct procentry64 procBuf[BUFFSIZE];
    //struct fdsinfo64 fileBuf[BUFFSIZE];
    int rCount=0,idx=0;
    pid_t *pidx = (pid_t *)malloc(sizeof(pid_t));
    *pidx = 0;
    do{
        pids = jarray;
        if (pids == NULL) {
            break;
        }
        if (jparentArray != NULL) {
            ppids  = jparentArray;
            if (ppids == NULL) {
                break;
            }
        }
        if (jstimesArray != NULL) {
            stimes  = jstimesArray;
            if (stimes == NULL) {
                break;
            }
        }
        rCount = getprocs64(procBuf,sizeof(struct procentry64),NULL,0,pidx,BUFFSIZE);
        //printf("rCount=%d,errno=%d\n",rCount,errno);
        for(idx=0;idx<rCount;idx++ ){
            
            if(procBuf[idx].pi_ppid == pid && procBuf[idx].pi_state == SACTIVE){
                printf("count:%d,pid:%d,ppid:%d,stime:%ld,state:%d\n",count,procBuf[idx].pi_pid,procBuf[idx].pi_ppid,procBuf[idx].pi_start,procBuf[idx].pi_state);
                if (count < arraySize) {
                    pids[count] = (long)(procBuf[idx].pi_pid);
                    printf("cp0,count:%d,pid:%d-->%ld\n",count,procBuf[idx].pi_pid,pids[count]);
                    if (ppids != NULL) {
                        // Store the parentPid
                        ppids[count] =  (long)(procBuf[idx].pi_ppid);
                        printf("cp1,count:%d,ppid:%d-->%ld\n",count,procBuf[idx].pi_ppid,ppids[count]);
                    }
                    if (stimes != NULL) {
                        // Store the process start time
                        stimes[count] = (long) (procBuf[idx].pi_start * 1000);
                        printf("cp2,count:%d,start:%d-->%ld\n",count,procBuf[idx].pi_start,stimes[count]);
                    }
                }
                count++;
                printf("cp3\n");
            }
        }
    }while(rCount > 0);
    
#endif
    // If more pids than array had size for; count will be greater than array size
    return count;
}

int main(int argc, char * argv[]){
    pid_t pid = (pid_t)0,ppid=(pid_t)0;
    long pids[20],ppids[20],starttime[20];
    long totalTime = 0L;
    long startTime = 0L;
    memset(pids,0,sizeof(long)*20);
    memset(ppids,0,sizeof(long)*20);
    memset(starttime,0,sizeof(long)*20);

    int cc =0;
    if(argc>1){
        pid = (pid_t) atoi(argv[1]);
    }
    if(pid !=0){
        cc = unix_getChildren(pid,pids,ppids,starttime,20);
        printf("Child of pid[%d]:%d\n",pid,cc);
        for(int x = 0;x<cc;x++){
            printf("idx:%d,pid:%ld,ppid:%ld,starttime:%ld\n",x,pids[x],ppids[x],starttime[x]);
        }
    }
    ppid = unix_getParentPidAndTimings(pid, &totalTime, &startTime);
    printf("Infoof:%d,ppid:%d,totaltime:%ld,startTime:%ld\n",pid,ppid,totalTime,startTime);
}