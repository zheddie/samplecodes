#include <stdio.h>
#include <sys/procfs.h>
#include <dirent.h>
#include <sys/types.h>

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
}

int unix_getChildren( pid_t pid, long jarray[],
                      long jparentArray[], long jstimesArray[],int arraySize) {
    DIR* dir;
    struct dirent* ptr;
    long* pids = NULL;
    long* ppids = NULL;
    long* stimes = NULL;
    int count = 0;

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
            //if(ppid>=0) printf("/proc/%d,ppid:%d\n",childpid,ppid);
            if (ppid >= 0 && (pid == 0 || ppid == pid)) {
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
    // If more pids than array had size for; count will be greater than array size
    return count;
}

int main(int argc, char * argv[]){
    pid_t pid = (pid_t)0;
    long pids[20],ppids[20],starttime[20];
    int cc =0;
    if(argc>1){
        pid = (pid_t) atoi(argv[1]);
    }
    if(pid !=0){
        cc = unix_getChildren(pid,pids,ppids,starttime,20);
        printf("Child of pid[%d]:%d\n",pid,cc);
        for(int x = 0;x<cc;x++){
            printf("idx:%d,pid:%ld,ppid:%ld,starttime:%ld\n",pids[x],ppids[x],starttime[x]);
        }
    }
}