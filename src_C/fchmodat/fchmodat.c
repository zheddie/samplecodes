#include <stdlib.h>
#include <stdio.h>
//#include <procinfo.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>


int main(int argc, char* argv[])
{
        char path[] = "/home/zhanggan/worktemp/afile";
        int rc = 0;
        int dirfd = open("/home/zhanggan/worktemp", O_RDONLY|O_DIRECTORY);
        printf("dirfd = %d\n", dirfd);
        rc = openat(dirfd, "afile", O_RDONLY);
        printf("rc = %d, errno = %d\n", rc, errno);
        rc = fchmodat(dirfd, path, S_IRUSR|S_IWUSR|S_IXUSR, 0);
        printf("rc = %d, errno = %d\n", rc, errno);

        close(dirfd);
        return 0;
}
