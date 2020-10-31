#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "memlayout.h"
#include "user_gui.h"
#include "gui.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

window startWindow;

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

int main(int argc, char *argv[])
{

    int caller = (int)argv[1];
    printf(1, "start window created for %d\n", caller);

    startWindow.width = 3 * START_ICON_WIDTH;
    startWindow.height = SCREEN_HEIGHT / 2;
    startWindow.initialPosition.xmin = 0;
    startWindow.initialPosition.xmax = startWindow.width;
    startWindow.initialPosition.ymin = SCREEN_HEIGHT - DOCK_HEIGHT - startWindow.height;
    startWindow.initialPosition.ymax = SCREEN_HEIGHT - DOCK_HEIGHT;
    startWindow.hasTitleBar = 0;
    createPopupWindow(&startWindow, caller);

    struct RGBA desktopColor;
    desktopColor.R = 66;
    desktopColor.G = 130;
    desktopColor.B = 244;
    desktopColor.A = 250;
    struct RGBA color;
    color.R = 219;
    color.G = 68;
    color.B = 55;
    color.A = 255;

    addButtonWidget(&startWindow, desktopColor, color, "button", 10, 150, 50, 25, buttonHandler);

    //int startTime=uptime();
    while (1)
    {
        updatePopupWindow(&startWindow);
    }
}