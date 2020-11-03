#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "memlayout.h"
#include "user_gui.h"
#include "user_window.h"
#include "user_handler.h"
#include "gui.h"
#include "msg.h"
//#include "defs.h"

window desktop;
char pathname[40];

int main(int argc, char *argv[])
{

    struct RGBA bgColor;

    desktop.width = 400;
    desktop.height = 300;
    desktop.hasTitleBar = 1;
    createWindow(&desktop, "desktop");

    bgColor.R = 255;
    bgColor.G = 255;
    bgColor.B = 255;
    bgColor.A = 250;
    addColorFillWidget(&desktop, bgColor, 0, 0, desktop.width, desktop.height, 0, emptyHandler);

    /*
    struct RGBA textColor;
    textColor.R = 2;
    textColor.G = 6;
    textColor.B = 5;
    textColor.A = 255;
    */

    //int file = -1;
    int pipefd[2];
    char buf;
    int cpid;

    if (pipe(pipefd) == -1)
    {
        //perror("pipe");
        exit();
    }
    printf(1, "pipe, %d, %d\n", pipefd[0], pipefd[1]);

    cpid = fork();
    if (cpid == -1)
    {
        //perror("fork");
        exit();
    }

    if (cpid == 0)
    {                     /* Child reads from pipe */
        close(pipefd[1]); /* Close unused write end */

        while (read(pipefd[0], &buf, 1) > 0)
            write(1, &buf, 1);

        write(1, "\n", 1);
        close(pipefd[0]);
        exit();
    }
    else
    {                     /* Parent writes argv[1] to pipe */
        close(pipefd[0]); /* Close unused read end */
        write(pipefd[1], "hello", 5);
        close(pipefd[1]); /* Reader will see EOF */
        wait();       /* Wait for child */
    }

    //addInputFieldWidget(&desktop, textColor, initial, 10, 50, 380, 240, inputHandler);

    while (1)
    {
        updateWindow(&desktop);
    }
}