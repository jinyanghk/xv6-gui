#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "memlayout.h"
#include "user_gui.h"
#include "gui.h"
//#include "defs.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main()
{

    window desktop;
    desktop.width=SCREEN_WIDTH;
    desktop.height=SCREEN_HEIGHT;
    UI_createWindow(&desktop, "desktop");
    struct RGBA desktopColor;
    desktopColor.R=66;
    desktopColor.G=130;
    desktopColor.B=244;
    desktopColor.A=250;
    drawFillRect(&desktop, desktopColor, 0, 0, desktop.width, desktop.height);

    //window desktop2;
    //desktop2.width=400;
    //desktop2.height=200;
    //UI_createWindow(&desktop2, "desktop2");

    int lastTime=0;
    while (1)
    {
        int currentTime=uptime();
        if (currentTime-lastTime>=2)
        {
            UI_updateWindow(&desktop);
            updateScreen();
            lastTime=currentTime;
        }
    }
}