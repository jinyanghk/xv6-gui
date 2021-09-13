#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "memlayout.h"
#include "user_gui.h"
#include "user_window.h"
#include "gui.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

window startWindow;

char *GUI_programs[] = {"shell", "editor", "explorer", "demo"};

void startProgramHandler(Widget *widget, message *msg)
{
    if (msg->msg_type == M_MOUSE_DBCLICK)
    {
        if (fork() == 0)
        {
            printf(1, "fork new process\n");
            char *argv2[] = {widget->context.button->text};
            exec(argv2[0], argv2);
            exit();
        }
    }
}

int main(int argc, char *argv[])
{

    int caller = (int)argv[1];
    //printf(1, "start window created for %d\n", caller);

    startWindow.width = 3 * START_ICON_WIDTH;
    startWindow.height = SCREEN_HEIGHT / 2;
    startWindow.initialPosition.xmin = 0;
    startWindow.initialPosition.xmax = startWindow.width;
    startWindow.initialPosition.ymin = SCREEN_HEIGHT - DOCK_HEIGHT - startWindow.height;
    startWindow.initialPosition.ymax = SCREEN_HEIGHT - DOCK_HEIGHT;
    startWindow.hasTitleBar = 0;
    createPopupWindow(&startWindow, caller);

    struct RGBA buttonColor;
    struct RGBA textColor;
    buttonColor.R = 244;
    buttonColor.G = 180;
    buttonColor.B = 0;
    buttonColor.A = 255;

    textColor.R = 0;
    textColor.G = 0;
    textColor.B = 0;
    textColor.A = 255;

    for (int i = 0; i < 4; i++)
    {
        addButtonWidget(&startWindow, textColor, buttonColor, GUI_programs[i], 20, 20 + 50 * i, 80, 30, 0, startProgramHandler);
    }

    while (1)
    {
        updatePopupWindow(&startWindow);
    }
}