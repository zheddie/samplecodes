#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
int
statMode(const char *path, int *mode)
{
	printf("zg.statMode().CP0,path=%s=\n",path);
    struct stat64 sb;
    int rt = stat64(path, &sb);
    if ( rt == 0) {

        *mode = sb.st_mode;
        printf("zg.statMode().CP1,path=%s=,mode=%d\n",path,*mode);
        return 1;
    }
    printf("zg.statMode().CP3,rt=%d,errno=%d\n",rt,errno);
    return 0;
}
