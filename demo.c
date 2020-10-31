#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "memlayout.h"
#include "user_gui.h"
#include "gui.h"

void buttonHandler(message *msg)
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
    createWindow(&desktop2, "Demo Program");

    struct RGBA color;
    color.R = 219;
    color.G = 68;
    color.B = 55;
    color.A = 255;
    drawString(&desktop2, 10, 20, "hello world", color, 200);

    struct RGBA desktopColor;
    desktopColor.R = 66;
    desktopColor.G = 130;
    desktopColor.B = 244;
    desktopColor.A = 250;
    addButtonWidget(&desktop2, desktopColor, color, "button", 10, 150, 50, 25, buttonHandler);

    int startTime=uptime();
    while (1)
    {
        if(uptime()-startTime>600) {
            drawString(&desktop2, 10, 40, "hello world again", color, 200);
        }
        updateWindow(&desktop2);
        
    }
}