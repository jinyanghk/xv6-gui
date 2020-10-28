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

    int width= win->width;
    int height= win->height;

    win->window_buf = malloc(width * height * 3);
    if (!win->window_buf)
    {
        return;
    }

    memset(win->window_buf, 100, height * width * 3);

    createWindow(win);
}