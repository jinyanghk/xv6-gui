#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "memlayout.h"
#include "user_gui.h"
//#include "defs.h"

int main()
{

    window desktop;
    desktop.position.xmin = 50;
    desktop.position.xmax = 250;
    desktop.position.ymin = 50;
    desktop.position.ymax = 120;
    UI_createWindow(&desktop, "desktop");

    window desktop2;
    desktop2.position.xmin = 100;
    desktop2.position.xmax = 500;
    desktop2.position.ymin = 100;
    desktop2.position.ymax = 280;
    UI_createWindow(&desktop2, "desktop");


    window desktop3;
    desktop3.position.xmin = 400;
    desktop3.position.xmax = 700;
    desktop3.position.ymin = 250;
    desktop3.position.ymax = 400;
    UI_createWindow(&desktop3, "desktop");

    int lastTime=0;
    while (1)
    {
        int currentTime=uptime();
        //printf(1, "uptime %d\n", currentTime);
        if (currentTime-lastTime>=2)
        {
            updateScreen();
            lastTime=currentTime;
        }
    }
}