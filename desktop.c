#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "memlayout.h"
#include "user_gui.h"
#include "gui.h"
#include "msg.h"
//#include "defs.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

window desktop;

struct RGBA desktopColor;
struct RGBA color;
int buttonCount = 0;

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

        //addButtonWidget(&desktop, desktopColor, color, "button", 10, 10+buttonCount*35, 50, 30, buttonHandler);
        //buttonCount++;
    }
}

void startWindowHandler(message *msg)
{
    int mouse_x = msg->params[0];
    int mouse_y = msg->params[1];
    if (msg->msg_type == M_MOUSE_LEFT_CLICK && mouse_x < START_ICON_WIDTH && mouse_y > SCREEN_HEIGHT - DOCK_HEIGHT)
    {

        if (fork() == 0)
        {
            printf(1, "fork new process\n");
            char *argv2[] = {"startWindow", (char*)desktop.handler};
            exec(argv2[0], argv2);
            exit();
        }

        //addButtonWidget(&desktop, desktopColor, color, "button", 10, 10+buttonCount*35, 50, 30, buttonHandler);
        //buttonCount++;
    }
}

void poweroffHandler(message *msg)
{
    if (msg->msg_type == M_MOUSE_DBCLICK)
    {
        GUI_turnoffScreen();
        //kill(1);
        //addButtonWidget(&desktop, desktopColor, color, "button", 10, 10+buttonCount*35, 50, 30, buttonHandler);
        //buttonCount++;
    }
}

int main(int argc, char *argv[])
{

    printf(1, "desktop at %x\n", &desktop);
    desktop.width = SCREEN_WIDTH;
    desktop.height = SCREEN_HEIGHT;
    desktop.initialPosition.xmin = 0;
    desktop.initialPosition.xmax = SCREEN_WIDTH;
    desktop.initialPosition.ymin = 0;
    desktop.initialPosition.ymax = SCREEN_HEIGHT;
    desktop.hasTitleBar = 0;
    createWindow(&desktop, "desktop");

    desktopColor.R = 66;
    desktopColor.G = 130;
    desktopColor.B = 244;
    desktopColor.A = 250;
    drawFillRect(&desktop, desktopColor, 0, 0, desktop.width, desktop.height);

    color.R = 219;
    color.G = 68;
    color.B = 55;
    color.A = 255;
    addButtonWidget(&desktop, desktopColor, color, "button", 10, 10, 50, 30, buttonHandler);

    addButtonWidget(&desktop, desktopColor, color, "start", 5, SCREEN_HEIGHT - 30, 60, 25, startWindowHandler);

    //window desktop2;
    //desktop2.width=400;
    //desktop2.height=200;
    //UI_createWindow(&desktop2, "desktop2");

    int lastTime = 0;
    while (1)
    {
        updateWindow(&desktop);
        int currentTime = uptime();
        if (currentTime - lastTime >= 1)
        {
            GUI_updateScreen();
            lastTime = currentTime;
        }
    }
}