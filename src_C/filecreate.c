#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#define FILE_MODE   (O_CREAT|O_RDWR |O_TRUNC)
 
char    buf1[] = "abcdefghij";
char    buf2[] = "ABCDEFGHIJ";
char    buf3[10];
 
int
main(void)
{
    int     fd;
    
    if ((fd = open("./created.txt", FILE_MODE)) < 0){
        printf("creat error");
        exit(1);
    }
    if (write(fd, buf1, 10) != 10){
        printf("buf1 write error");
        exit(2);
    }
    /* offset now = 10 */
    lseek(fd,0,0);
    printf ("%zd\n", read(fd, buf3, 2));
    printf("buf3=%s\n",buf3);
    close(fd);
    exit(0);
}