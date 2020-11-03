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

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
window desktop;

struct RGBA desktopColor;
struct RGBA color;
int buttonCount = 0;

void buttonHandler(Widget *widget, message *msg)
{
    if (msg->msg_type == M_MOUSE_DBCLICK)
    {
        RGBA clickedColor;
        clickedColor.R = 76;
        clickedColor.G = 160;
        clickedColor.B = 255;
        clickedColor.A = 255;
        widget->context.button->bg_color = clickedColor;
        if (fork() == 0)
        {
            printf(1, "fork new process\n");
            //char *argv2[] = {"text_editor"};
            char *argv2[] = {"explorer"};
            exec(argv2[0], argv2);
            exit();
        }

        //addButtonWidget(&desktop, desktopColor, color, "button", 10, 10+buttonCount*35, 50, 30, buttonHandler);
        //buttonCount++;
    }
}
void button2Handler(Widget *widget, message *msg)
{
    if (msg->msg_type == M_MOUSE_DBCLICK)
    {
        RGBA clickedColor;
        clickedColor.R = 76;
        clickedColor.G = 160;
        clickedColor.B = 255;
        clickedColor.A = 255;
        widget->context.button->bg_color = clickedColor;
        if (fork() == 0)
        {
            printf(1, "fork new process\n");
            char *argv2[] = {"shell"};
            exec(argv2[0], argv2);
            exit();
        }

        //addButtonWidget(&desktop, desktopColor, color, "button", 10, 10+buttonCount*35, 50, 30, buttonHandler);
        //buttonCount++;
    }
}

void startWindowHandler(Widget *widget, message *msg)
{
    int mouse_x = msg->params[0];
    int mouse_y = msg->params[1];
    if (msg->msg_type == M_MOUSE_LEFT_CLICK && mouse_x < START_ICON_WIDTH && mouse_y > SCREEN_HEIGHT - DOCK_HEIGHT)
    {

        if (fork() == 0)
        {
            printf(1, "fork new process\n");
            char *argv2[] = {"startWindow", (char *)desktop.handler};
            exec(argv2[0], argv2);
            exit();
        }

        //addButtonWidget(&desktop, desktopColor, color, "button", 10, 10+buttonCount*35, 50, 30, buttonHandler);
        //buttonCount++;
    }
}

void inputHandler(Widget *w, message *msg)
{
    int mouse_x = msg->params[0];
    int mouse_y = msg->params[1];
    int width = w->position.xmax - w->position.xmin;
    //int height = w->position.ymax - w->position.ymin;
    //int charPerLine = width / CHARACTER_WIDTH;
    int charCount = strlen(w->context.inputfield->text);
    if (msg->msg_type == M_MOUSE_LEFT_CLICK)
    {

        int mouse_char_y = (mouse_y - w->position.ymin) / CHARACTER_HEIGHT;
        int mouse_char_x = (mouse_x - w->position.xmin) / CHARACTER_WIDTH;
        //int new_pos = mouse_char_y * charPerLine + mouse_char_x;
        int new_pos=getInputOffsetFromMousePosition(w->context.inputfield->text, width, mouse_char_x, mouse_char_y);
        if(new_pos> charCount) new_pos=charCount;
        w->context.inputfield->current_pos = new_pos;
    }
    else if (msg->msg_type == M_KEY_DOWN)
    {
        inputFieldKeyHandler(w, msg);
    }
}

int main(int argc, char *argv[])
{

    desktop.width = SCREEN_WIDTH;
    desktop.height = SCREEN_HEIGHT;
    desktop.initialPosition.xmin = 0;
    desktop.initialPosition.xmax = SCREEN_WIDTH;
    desktop.initialPosition.ymin = 0;
    desktop.initialPosition.ymax = SCREEN_HEIGHT;
    desktop.hasTitleBar = 0;
    createWindow(&desktop, "desktop");

    desktopColor.R = 66;
    desktopColor.G = 130;
    desktopColor.B = 244;
    desktopColor.A = 250;
    addColorFillWidget(&desktop, desktopColor, 0, 0, desktop.width, desktop.height,0, emptyHandler);

    color.R = 219;
    color.G = 68;
    color.B = 55;
    color.A = 255;
    //drawIcon(&desktop, 10, 10, 3, color);

    addButtonWidget(&desktop, desktopColor, color, "button", 10, 40, 50, 30, 0, buttonHandler);

    addButtonWidget(&desktop, desktopColor, color, "button", 10, 80, 50, 30, 0, button2Handler);

    addButtonWidget(&desktop, desktopColor, color, "start", 5, SCREEN_HEIGHT - 30, 60, 25, 0, startWindowHandler);

    //addInputFieldWidget(&desktop, color, "button is \na\nlong\nline", 100, 40, 100, 100, 0, inputHandler);


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