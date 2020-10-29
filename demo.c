#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "memlayout.h"
#include "user_gui.h"
#include "gui.h"

int main()
{

    window desktop2;
    desktop2.width = 400;
    desktop2.height = 200;
    UI_createWindow(&desktop2, "demo");

    int lastTime = 0;
    while (1)
    {
        int currentTime = uptime();
        if (currentTime - lastTime >= 2)
        {
            struct RGBA color;
            color.R = 219;
            color.G = 68;
            color.B = 55;
            color.A =255;
            drawString(&desktop2, 10, 20, "hello world", color, 200);
            //UI_updateWindow(&desktop2);
            lastTime = currentTime;
        }
    }
}