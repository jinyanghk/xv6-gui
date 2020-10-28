#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "memlayout.h"
#include "user_gui.h"
#include "gui.h"
//#include "defs.h"

int main()
{

    window desktop;
    desktop.width=200;
    desktop.height=100;
    UI_createWindow(&desktop, "desktop");

    window desktop2;
    desktop2.width=400;
    desktop2.height=200;
    UI_createWindow(&desktop2, "desktop");

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