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
int sh_pid, rfd, wfd;
int gui2sh_fd[2], sh2gui_fd[2];
char init_string[] = "$ ";
int commandWidgetId;
int totallines = 0;

#define READBUFFERSIZE 1024
char read_buf[READBUFFERSIZE];
struct RGBA commandColor;
struct RGBA textColor;

void create_shell(int *p_pid, int *p_rfd, int *p_wfd)
{
    char *sh_argv[] = {"sh", 0, 0};
    char rfd[2], wfd[2];

    memset(rfd, 0, sizeof(char) * 2);
    memset(wfd, 0, sizeof(char) * 2);
    sh_argv[1] = rfd;
    sh_argv[2] = wfd;

    printf(1, "init pipe: starting pipe\n");
    if (pipe(gui2sh_fd) != 0)
    {
        printf(1, "init gui->sh pipe: pipe() failed\n");
        exit();
    }
    if (pipe(sh2gui_fd) != 0)
    {
        printf(1, "init sh->gui pipe: pipe() failed\n");
        exit();
    }
    printf(1, "init pipe: pipe is ok\n");
    printf(1, "pipe gui2sh, %d, %d\n", gui2sh_fd[0], gui2sh_fd[1]);
    printf(1, "pipe sh2gui, %d, %d\n", sh2gui_fd[0], sh2gui_fd[1]);

    printf(1, "init sh: starting sh\n");
    *p_pid = fork();
    if (*p_pid < 0)
    {
        printf(1, "init sh: fork failed\n");
        close(gui2sh_fd[0]);
        close(gui2sh_fd[1]);
        close(sh2gui_fd[0]);
        close(sh2gui_fd[1]);
        exit();
    }
    else if (*p_pid == 0)
    {
        close(gui2sh_fd[1]);
        rfd[0] = (char)gui2sh_fd[0];
        close(sh2gui_fd[0]);
        wfd[0] = (char)sh2gui_fd[1];
        exec("sh", sh_argv);
        printf(1, "init sh: exec sh failed\n");
        exit();
    }
    else
    {
        close(gui2sh_fd[0]);
        *p_wfd = gui2sh_fd[1];
        close(sh2gui_fd[1]);
        *p_rfd = sh2gui_fd[0];
    }
}

int readCommand = -1;

void inputHandler(Widget *w, message *msg)
{

    int mouse_x = msg->params[0];
    int mouse_y = msg->params[1];
    int width = w->position.xmax - w->position.xmin;
    int height = w->position.ymax - w->position.ymin;
    //int charPerLine = width / CHARACTER_WIDTH;
    int charCount = strlen(w->context.inputfield->text);
    if (msg->msg_type == M_MOUSE_LEFT_CLICK)
    {

        int mouse_char_y = (mouse_y - w->position.ymin) / CHARACTER_HEIGHT;
        int mouse_char_x = (mouse_x - w->position.xmin) / CHARACTER_WIDTH;
        int new_pos = getInputOffsetFromMousePosition(w->context.inputfield->text, width, mouse_char_x, mouse_char_y);
        if (new_pos > charCount)
            new_pos = charCount;
        w->context.inputfield->current_pos = new_pos;
    }
    else if (msg->msg_type == M_KEY_DOWN)
    {
        int c = msg->params[0];
        char buffer[MAX_LONG_STRLEN];
        if (c == '\n' && charCount > 0)
        {
            debugPrintWidgetList(&desktop);
            //printf(1, "inputfield content\n");
            //printf(1,w->context.inputfield->text);
            memset(buffer, 0, MAX_LONG_STRLEN);
            strcpy(buffer, w->context.inputfield->text);
            //printf(1, "shell read from %d writing to %d, %s\n", rfd, wfd, buffer);

            if (write(wfd, buffer, strlen(buffer)) != strlen(buffer))
            {
                printf(1, "gui pipe write: failed\n");
            }
            else
            {   
                memset(read_buf, 0, READBUFFERSIZE);
                int n;
                // Read the result until get the initial string "$ "

                while (1)
                {
                    if ((n = read(rfd, read_buf, READBUFFERSIZE)) > 0)
                    {
                        //printf(1, "readed back %d \n", n);
                        if (read_buf[n - 2] == init_string[0] && read_buf[n - 1] == init_string[1])
                        {
                            memset(read_buf + n - 2, '\0', 1);
                            break;
                        }
                    }
                }
            }
        

            int commandLindCount = getMouseYFromOffset(buffer, width, strlen(buffer)) + 1;
            removeWidget(&desktop, commandWidgetId);
            addTextWidget(&desktop, commandColor, buffer, 0, totallines * CHARACTER_HEIGHT, width, commandLindCount * CHARACTER_HEIGHT, 1, emptyHandler);
            totallines += commandLindCount;


            int respondLineCount = getMouseYFromOffset(read_buf, width, strlen(read_buf));
            readCommand = addTextWidget(&desktop, textColor, read_buf, 0, totallines * CHARACTER_HEIGHT, width, respondLineCount * CHARACTER_HEIGHT, 1, emptyHandler);
            totallines += respondLineCount;

            
            commandWidgetId = addInputFieldWidget(&desktop, commandColor, "", 0, totallines * CHARACTER_HEIGHT, width, height, 1, inputHandler);
            printf(1, "total line is %d high\n", totallines * CHARACTER_HEIGHT);
            if (totallines * CHARACTER_HEIGHT - desktop.height > 0)
            {
                desktop.scrollOffset = totallines * CHARACTER_HEIGHT - desktop.height + CHARACTER_HEIGHT;
                //desktop.needsRepaint = 1;
            }
        }
        else
        {
            inputFieldKeyHandler(w, msg);
        }
    }
}

int main(int argc, char *argv[])
{

    struct RGBA bgColor;

    desktop.width = 400;
    desktop.height = 407;
    desktop.hasTitleBar = 1;
    createWindow(&desktop, "desktop");

    bgColor.R = 255;
    bgColor.G = 255;
    bgColor.B = 255;
    bgColor.A = 250;
    addColorFillWidget(&desktop, bgColor, 0, 0, desktop.width, desktop.height, 0, emptyHandler);

    textColor.R = 2;
    textColor.G = 6;
    textColor.B = 5;
    textColor.A = 255;
    commandColor.R = 66;
    commandColor.G = 130;
    commandColor.B = 245;
    commandColor.A = 255;

    create_shell(&sh_pid, &rfd, &wfd);

    commandWidgetId = addInputFieldWidget(&desktop, commandColor, "", 0, 0, desktop.width, desktop.height, 1, inputHandler);

    while (1)
    {

        updateWindow(&desktop);
    }
}