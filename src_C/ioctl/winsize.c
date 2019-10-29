#include<stdio.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<termios.h>

int main()
{
    struct winsize size;
    // Get current windows size
    ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
    printf("window width: %d\n", size.ws_col);
    printf("window length: %d\n", size.ws_row);

    size.ws_col += 20;
    size.ws_row += 20;

    // Set new windows size
    ioctl(STDIN_FILENO, TIOCSWINSZ, &size);
    printf("window width: %d\n", size.ws_col);
    printf("window length: %d\n", size.ws_row);
    return 0;
}
