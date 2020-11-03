#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "memlayout.h"
#include "user_gui.h"
#include "user_window.h"
#include "user_handler.h"
#include "gui.h"
#include "msg.h"
#include "fs.h"
#include "stat.h"
//#include "defs.h"

window desktop;
char pathname[40];
char buf[MAX_LONG_STRLEN];
struct RGBA textColor;

void buttonHandler(Widget *widget, message *msg)
{
    if (msg->msg_type == M_MOUSE_DBCLICK)
    {
        
        //char command[MAX_SHORT_STRLEN];
        //strcpy(command, widget->context.text->text);
        if (fork() == 0)
        {
            char *argv2[] = {widget->context.text->text};
            exec(argv2[0], argv2);
            exit();
        }

    }
}

char *
fmtname(char *path)
{
    static char buf[DIRSIZ + 1];
    char *p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf + strlen(p), '\0', 1);
    //memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
    return buf;
}

void gui_ls(char *path)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    int lineCount = 0;

    if ((fd = open(path, 0)) < 0)
    {
        printf(2, "ls: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        printf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_FILE:
        addTextWidget(&desktop, textColor, fmtname(path), 1, lineCount * CHARACTER_HEIGHT, 200, CHARACTER_HEIGHT, 1, emptyHandler);
        lineCount++;
        //printf(1, "%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
        break;

    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf(1, "ls: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0)
            {
                printf(1, "ls: cannot stat %s\n", buf);
                continue;
            }
            //printf(1, "%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
            if (st.type == T_FILE)
            {
                addTextWidget(&desktop, textColor, fmtname(buf), 1, lineCount * CHARACTER_HEIGHT, 200, CHARACTER_HEIGHT, 1, buttonHandler);
                lineCount++;
            }
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[])
{

    struct RGBA bgColor;

    desktop.width = 400;
    desktop.height = 300;
    desktop.hasTitleBar = 1;
    createWindow(&desktop, "explorer");

    bgColor.R = 255;
    bgColor.G = 255;
    bgColor.B = 255;
    bgColor.A = 250;
    textColor.R = 2;
    textColor.G = 6;
    textColor.B = 5;
    textColor.A = 255;
    addColorFillWidget(&desktop, bgColor, 0, 0, desktop.width, desktop.height, 0, emptyHandler);

    gui_ls(".");
    //addTextWidget(&desktop, textColor, "demo", 1, 1, 200, CHARACTER_HEIGHT*2, 1, buttonHandler);

    while (1)
    {
        updateWindow(&desktop);
    }
}