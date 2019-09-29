#include <stdio.h>
#include <sys/stat.h>
int
statMode(const char *path, int *mode)
{
#if defined(__MVS__)
	struct stat sb;
	if (lstat(path, &sb) == 0) {
		/*JSE-2652: Identifying if the path passed is an external link
         * Only applicable on zOS
         */
		if (S_ISEXTL(sb.st_mode, sb.st_genvalue) == 0) {
			if (stat(path, &sb) == 0) {
				*mode = sb.st_mode;
				return 1;
			}
		} else {
				*mode = sb.st_mode;
				return 1;
		}
	}
#else
    struct stat64 sb;
    if (stat64(path, &sb) == 0) {
        *mode = sb.st_mode;
        return 1;
    }
#endif
    return 0;
}



int main(int args, char * argv[]){
	int mode  = 0;
	statMode(argv[1],&mode);
	int fmt = mode & S_IFMT;
    printf("fmt=%X\n",fmt);
	return(0);
}