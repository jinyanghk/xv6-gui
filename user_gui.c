#include "types.h"
#include "stat.h"
//#include "color.h"
#include "msg.h"
#include "user.h"
#include "fcntl.h"
#include "user_gui.h"
#include "character.h"
#include "fs.h"
#include "gui.h"

void UI_createWindow(window *win, const char *title)
{

    if (win->position.xmin < 0 || win->position.xmax > MAX_WIDTH || win->position.ymin < 0 || win->position.ymax > MAX_HEIGHT)
    {
        return; 
    }
    int width= win->position.xmax- win->position.xmin;
    int height= win->position.ymax - win->position.ymin;

    win->window_buf = malloc(width * height * 3);
    win->title_buf = malloc(width*TITLE_HEIGHT*3);
    if (!win->window_buf || ! win->title_buf)
    {
        return;
    }

    memset(win->window_buf, 100, height * width * 3);
    memset(win->title_buf, 20, TITLE_HEIGHT * width * 3);

    createWindow(win);
}