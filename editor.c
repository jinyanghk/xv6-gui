#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "memlayout.h"
#include "user_gui.h"
#include "user_window.h"
#include "user_handler.h"
#include "gui.h"
#include "msg.h"
//#include "defs.h"

window desktop;
char filename[40];

void buttonHandler(Widget *widget, message *msg)
{
    if (msg->msg_type == M_MOUSE_LEFT_CLICK)
    {
        RGBA clickedColor;
        clickedColor.R = 76;
        clickedColor.G = 160;
        clickedColor.B = 255;
        clickedColor.A = 255;
        widget->context.button->bg_color = clickedColor;
        int file = open(filename, 1);
        int i;
        for (i = desktop.widgetlisthead; i !=-1; i=desktop.widgets[i].next)
        {
            if (desktop.widgets[i].type == INPUTFIELD)
            {
                break;
            }
        }
        write(file, desktop.widgets[i].context.inputfield->text, 512);
        close(file);
    }
}

void inputHandler(Widget *w, message *msg)
{
    int mouse_x = msg->params[0];
    int mouse_y = msg->params[1];
    int width = w->position.xmax - w->position.xmin;
    printf(1, "mouse at %d, %d\n", mouse_x, mouse_y);
    //int height = w->position.ymax - w->position.ymin;
    //int charPerLine = width / CHARACTER_WIDTH;
    int charCount = strlen(w->context.inputfield->text);
    if (msg->msg_type == M_MOUSE_LEFT_CLICK)
    {

        int mouse_char_y = (mouse_y - w->position.ymin) / CHARACTER_HEIGHT;
        int mouse_char_x = (mouse_x - w->position.xmin) / CHARACTER_WIDTH;
        int new_pos = getInputOffsetFromMousePosition(w->context.inputfield->text, width, mouse_char_x, mouse_char_y);
        if (new_pos > charCount)
            new_pos = charCount;
        w->context.inputfield->current_pos = new_pos;
    }
    else if (msg->msg_type == M_KEY_DOWN)
    {
        inputFieldKeyHandler(w, msg);
    }
}

int main(int argc, char *argv[])
{

    struct RGBA bgColor;
    struct RGBA textColor;
    desktop.width = 400;
    desktop.height = 300;
    desktop.hasTitleBar = 1;
    createWindow(&desktop, "text editor");

    bgColor.R = 255;
    bgColor.G = 255;
    bgColor.B = 255;
    bgColor.A = 250;
    addColorFillWidget(&desktop, bgColor, 0, 0, desktop.width, desktop.height, 0, buttonHandler);

    textColor.R = 2;
    textColor.G = 6;
    textColor.B = 5;
    textColor.A = 255;
    //drawIcon(&desktop, 10, 10, 3, color);

    int file = -1;
    if (argc > 1)
    {
        strcpy(filename, argv[1]);
        file = open(filename, O_RDWR);
    }
    else
    {
        strcpy(filename, "/new.txt");
        file = open(filename, O_CREATE);
    }
            

    addButtonWidget(&desktop, textColor, bgColor, "save", 10, 10, 50, 20, 0, buttonHandler);

    char initial[MAX_LONG_STRLEN]="Start typing...";
    memset(initial, 0, MAX_LONG_STRLEN);
    if (file >= 0) {
        read(file, initial, MAX_LONG_STRLEN);
        close(file);
    }

    addInputFieldWidget(&desktop, textColor, initial, 10, 50, 380, 240, 1, inputHandler);

    while (1)
    {
        updateWindow(&desktop);
    }
}