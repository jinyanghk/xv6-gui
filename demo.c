#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "memlayout.h"
#include "user_gui.h"
#include "gui.h"

int main()
{

    printf(1, "demo created\n");

    window desktop2;
    desktop2.width = 400;
    desktop2.height = 200;
    UI_createWindow(&desktop2, "demo");

    struct RGBA color;
    color.R = 219;
    color.G = 68;
    color.B = 55;
    color.A = 255;
    drawString(&desktop2, 10, 20, "hello world", color, 200);

    int startTime=uptime();
    while (1)
    {
        if(uptime()-startTime>600) {
            drawString(&desktop2, 10, 40, "hello world again", color, 200);
        }
        UI_updateWindow(&desktop2);
        
    }
}