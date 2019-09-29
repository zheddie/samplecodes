#include <sys/types.h>
#ifdef OSX
#include <sys/proc_info.h>
#else
#include <procinfo.h>
#endif
#include <unistd.h>
#include <stdio.h>

#define ENV_MAX (1024*1024)

int main(int argc, char * argv[])
{
    struct procentry64 pe;
    char buffer[ENV_MAX];
    if(argc < 1) return(-1);
    pe.pi_pid = atoi(argv[1]);
    int rc = getevars(&pe, sizeof(pe), buffer, sizeof(buffer));
    if (rc != 0) {
        perror("getevars");
        return 1;
    }
    buffer[ENV_MAX-2] = buffer[ENV_MAX-1] = 0;
    char *cur = buffer;
    char *end = buffer+sizeof(buffer);
    while ((*cur) && (cur < end)) {
        int len = puts(cur);
        cur += len;
    }
    return 0;
}
