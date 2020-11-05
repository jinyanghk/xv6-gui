#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "memlayout.h"
#include "user_gui.h"
#include "user_window.h"
#include "gui.h"

void buttonHandler(Widget* widget, message *msg)
{
    if (msg->msg_type == M_MOUSE_DBCLICK)
    {
        if (fork() == 0)
        {
            printf(1, "fork new process\n");
            char *argv2[] = {"demo"};
            exec(argv2[0], argv2);
            exit();
        }
    }
}

int main()
{

    printf(1, "demo created\n");

    window desktop2;
    desktop2.width = 400;
    desktop2.height = 200;
    desktop2.hasTitleBar = 1;
    createWindow(&desktop2, "Demo Program");

    struct RGBA color;
    color.R = 219;
    color.G = 68;
    color.B = 55;
    color.A = 255;
    drawString(&desktop2, "abcdefghijilmnopqrstuvwxyz", color, 10, 20, 200, 100);

    struct RGBA desktopColor;
    desktopColor.R = 66;
    desktopColor.G = 100;
    desktopColor.B = 24;
    desktopColor.A = 200;
    addColorFillWidget(&desktop2, desktopColor, 0, 0, desktop2.width, desktop2.height, 0, emptyHandler);
    addTextWidget(&desktop2, color, "button \n      button \n            button", 10, 150, 200, 70, 1, buttonHandler);
    

    int startTime=uptime();
    while (desktop2.handler!=-1)
    {
        if(uptime()-startTime>50) {
            desktop2.scrollOffsetY +=CHARACTER_HEIGHT;
            desktop2.needsRepaint=1;
            startTime=uptime();
        }
        updateWindow(&desktop2);
        
    }
}