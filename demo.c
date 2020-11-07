#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "memlayout.h"
#include "user_gui.h"
#include "user_window.h"
#include "gui.h"

int main()
{

    window programWindow;
    programWindow.width = 400;
    programWindow.height = 200;
    programWindow.hasTitleBar = 1;
    createWindow(&programWindow, "demo");

    struct RGBA color;
    color.R = 219;
    color.G = 68;
    color.B = 55;
    color.A = 255;

    struct RGBA desktopColor;
    desktopColor.R = 66;
    desktopColor.G = 100;
    desktopColor.B = 24;
    desktopColor.A = 200;
    addColorFillWidget(&programWindow, desktopColor, 0, 0, programWindow.width, programWindow.height, 0, emptyHandler);

    int birdWidth=10*CHARACTER_WIDTH;
    int birdHeight=4*CHARACTER_HEIGHT;

    addTextWidget(&programWindow, color, 
"    __\n___( o)>\n\\ <_. ) \n `---\'  ", 0, programWindow.height/2-2*CHARACTER_HEIGHT, birdWidth,  birdHeight, 1, emptyHandler);
    

    int startTime=uptime();
    int yDirection=1;
    while (1)
    {
        if (uptime() - startTime > 20)
        {
            if(programWindow.scrollOffsetX<-programWindow.width) {
                programWindow.scrollOffsetX=birdWidth;
            }
            programWindow.scrollOffsetX -= 2 * CHARACTER_WIDTH;
            
            if(programWindow.scrollOffsetY>CHARACTER_HEIGHT) {
                yDirection= -1;
            }
            if(programWindow.scrollOffsetY<-CHARACTER_HEIGHT) {
                yDirection= 1;
            }
            programWindow.scrollOffsetY+=yDirection*CHARACTER_HEIGHT;
            
            programWindow.needsRepaint = 1;
            startTime = uptime();
        }
        updateWindow(&programWindow);
        
    }
}