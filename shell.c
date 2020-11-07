#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "memlayout.h"
#include "user_gui.h"
#include "user_window.h"
#include "user_handler.h"
#include "gui.h"
#include "msg.h"

//for talking to the shell
int sh_pid, rfd, wfd;
int gui2sh_fd[2], sh2gui_fd[2];
char init_string[] = "$ ";
#define READBUFFERSIZE 1024
char read_buf[READBUFFERSIZE];

window programWindow;
int commandWidgetId;
int totallines = 0;
struct RGBA commandColor;
struct RGBA textColor;

int inputOffset = 10;

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

/*
//for creating a scrollbar, needs update
void scrollBarHandler(Widget *w, message *msg)
{
    if (msg->msg_type == M_MOUSE_LEFT_CLICK)
    {
        int mouse_y = msg->params[1];
        int maximumOffset = getScrollableTotalHeight(&programWindow) - programWindow.height;
        programWindow.scrollOffset = ((float)mouse_y / programWindow.height) * maximumOffset;

        int scrollableHeight = 20;
        int startHeight = ((float)mouse_y / programWindow.height) * (programWindow.height - scrollableHeight);

        printf(1, "start %d, scrollbar height %d\n", startHeight, scrollableHeight);
        if (scrollBallId != -1)
        {
            removeWidget(&programWindow, scrollBallId);
        }
        scrollBallId = addColorFillWidget(&programWindow, commandColor, programWindow.width - scrollableHeight, startHeight, scrollableHeight, scrollableHeight, 0, emptyHandler);
    }
}
*/
/*
void readFromShell()
{
    int n;
    // Read the result until get the initial string "$ "
    memset(read_buf, 0, READBUFFERSIZE);
    while ((n = read(rfd, read_buf, READBUFFERSIZE)) > 0)
    {

        if (read_buf[n - 2] == init_string[0] && read_buf[n - 1] == init_string[1])
        {
            memset(read_buf + n - 2, '\0', 1);
            break;
        }
        memset(read_buf, 0, READBUFFERSIZE);
    }
}
*/

void inputHandler(Widget *w, message *msg)
{

    int width = w->position.xmax - w->position.xmin;
    int height = w->position.ymax - w->position.ymin;
    int charCount = strlen(w->context.inputfield->text);
    if (msg->msg_type == M_MOUSE_LEFT_CLICK)
    {
        inputMouseLeftClickHandler(w, msg);
    }
    else if (msg->msg_type == M_KEY_DOWN)
    {
        int c = msg->params[0];
        char buffer[MAX_LONG_STRLEN];
        if (c == '\n' && charCount > 0)
        {
            memset(buffer, 0, MAX_LONG_STRLEN);
            strcpy(buffer, w->context.inputfield->text);

            if (write(wfd, buffer, strlen(buffer)) != strlen(buffer))
            {
                printf(1, "gui pipe write: failed\n");
            }
            else
            {
                int n;
                // Read the result until get the initial string "$ "
                memset(read_buf, 0, READBUFFERSIZE);
                while ((n = read(rfd, read_buf, READBUFFERSIZE)) > 0)
                {
                    //printf(1, "reading back\n");
                    //printf(1, read_buf);
                    if (read_buf[n - 2] == init_string[0] && read_buf[n - 1] == init_string[1])
                    {
                        printf(1, read_buf);
                        memset(read_buf + n - 2, '\0', 1);

                        break;
                    }
                    //memset(read_buf, 0, READBUFFERSIZE);
                }
            }

            int respondLineCount = getMouseYFromOffset(read_buf, width, strlen(read_buf));
            readCommand = addTextWidget(&programWindow, textColor, read_buf, inputOffset, inputOffset + totallines * CHARACTER_HEIGHT, width, respondLineCount * CHARACTER_HEIGHT, 1, emptyHandler);
            totallines += respondLineCount;

            int commandLindCount = getMouseYFromOffset(buffer, width, strlen(buffer)) + 1;
            removeWidget(&programWindow, commandWidgetId);
            addTextWidget(&programWindow, commandColor, buffer, inputOffset, inputOffset + totallines * CHARACTER_HEIGHT, width, commandLindCount * CHARACTER_HEIGHT, 1, emptyHandler);
            totallines += commandLindCount;

            commandWidgetId = addInputFieldWidget(&programWindow, commandColor, "", inputOffset, inputOffset + totallines * CHARACTER_HEIGHT, width, CHARACTER_HEIGHT, 1, inputHandler);

            int maximumOffset = getScrollableTotalHeight(&programWindow) - programWindow.height;
            if (maximumOffset > 0)
            {
                programWindow.scrollOffsetY = maximumOffset;
            }
        }
        else
        {
            inputFieldKeyHandler(w, msg);
            //grow the height of the input field as we type
            //may not be universal behavior for all input field
            int newHeight = CHARACTER_HEIGHT * (getMouseYFromOffset(w->context.inputfield->text, width, strlen(w->context.inputfield->text)) + 1);
            if (newHeight > height)
            {
                w->position.ymax = w->position.ymin + newHeight;
            }
        }
    }
}

int main(int argc, char *argv[])
{

    struct RGBA bgColor;

    programWindow.width = 400;
    programWindow.height = 400;
    programWindow.hasTitleBar = 1;
    createWindow(&programWindow, "shell");

    bgColor.R = 255;
    bgColor.G = 255;
    bgColor.B = 255;
    bgColor.A = 250;
    addColorFillWidget(&programWindow, bgColor, 0, 0, programWindow.width, programWindow.height, 0, emptyHandler);

    textColor.R = 2;
    textColor.G = 6;
    textColor.B = 5;
    textColor.A = 255;
    commandColor.R = 66;
    commandColor.G = 130;
    commandColor.B = 245;
    commandColor.A = 255;

    create_shell(&sh_pid, &rfd, &wfd);

    commandWidgetId = addInputFieldWidget(&programWindow, commandColor, "", inputOffset, inputOffset, programWindow.width - 2 * inputOffset, CHARACTER_HEIGHT, 1, inputHandler);

    while (1)
    {
        updateWindow(&programWindow);
    }
}