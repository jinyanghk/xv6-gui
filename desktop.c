#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "memlayout.h"
#include "user_gui.h"
//#include "defs.h"

int main()
{

    window desktop;
    desktop.position.xmin = 0;
    desktop.position.xmax = MAX_WIDTH;
    desktop.position.ymin = 0;
    desktop.position.ymax = MAX_HEIGHT;
    UI_createWindow(&desktop, "desktop");

    window desktop2;
    desktop2.position.xmin = 100;
    desktop2.position.xmax = 500;
    desktop2.position.ymin = 150;
    desktop2.position.ymax = 280;
    UI_createWindow(&desktop2, "desktop");

    int lastTime=0;
    while (1)
    {
        int currentTime=uptime();
        printf(1, "uptime %d\n", currentTime);
        if (currentTime-lastTime>=2)
        {
            updateScreen();
            lastTime=currentTime;
        }
    }
}