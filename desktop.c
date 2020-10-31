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
int buttonCount=0;

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

int main()
{

    
    desktop.width = SCREEN_WIDTH;
    desktop.height = SCREEN_HEIGHT;
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