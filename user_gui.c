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

    int width = win->width;
    int height = win->height;

    win->window_buf = malloc(width * height * 3);
    if (!win->window_buf)
    {
        return;
    }

    memset(win->window_buf, 100, height * width * 3);

    createWindow(win, title);
}

void UI_closeWindow(window *win)
{

    free(win->window_buf);

    closeWindow(win);
}

void UI_updateWindow(window *win)
{
    message msg;
    if (getMessage(win->handler, &msg)==0)
    {
        if(msg.msg_type!=0) {
            printf(1, "message is %d\n", msg.msg_type);
            printf(1, "mouse at %d, %d\n", msg.params[0], msg.params[1]);
        }
        if (msg.msg_type == WM_WINDOW_CLOSE)
        {
            UI_closeWindow(win);
        }
        if (msg.msg_type == M_MOUSE_DBCLICK && msg.params[0]<100 && msg.params[1]<100)
        {
            if (fork() == 0)
            {
                char *argv2[] = { "demo"};
                exec(argv2[0], argv2);
                exit();
            }
        }
    }
    return;
}

void drawPointAlpha(RGB *color, RGBA origin) {
    float alpha;
    if (origin.A == 255) {
        color->R = origin.R;
        color->G = origin.G;
        color->B = origin.B;
        return;
    }
    if (origin.A == 0) {
        return;
    }
    alpha = (float) origin.A / 255;
    color->R = color->R * (1 - alpha) + origin.R * alpha;
    color->G = color->G * (1 - alpha) + origin.G * alpha;
    color->B = color->B * (1 - alpha) + origin.B * alpha;
}

int drawCharacter(window *win, int x, int y, char ch, RGBA color) {
    int i, j;
    RGB *t;
    int ord = ch - 0x20;
    if (ord < 0 || ord >= (CHARACTER_NUMBER - 1)) {
        return -1;
    }
    for (i = 0; i < CHARACTER_HEIGHT; i++) {
        if (y + i > win->height) {
            break;
        }
        if (y + i < 0) {
            continue;
        }
        for (j = 0; j < CHARACTER_WIDTH; j++) {
            if (character[ord][i][j] == 1) {
                if (x + j > win->width) {
                    break;
                }
                if (x + j < 0) {
                    continue;
                }
                t = win->window_buf + (y + i) * win->width + x + j;
                drawPointAlpha(t, color);
            }
        }
    }
    return CHARACTER_WIDTH;
}

void drawString(window *win, int x, int y, char *str, RGBA color, int width) {
    int offset_x = 0;

    while (*str != '\0') {
        if (x + offset_x >= win->width || offset_x >= width) { // if too long
            break;
        }
        offset_x += drawCharacter(win, x + offset_x, y, *str, color);
        str++;
    }
}

void drawImage(window *win, RGBA *img, int x, int y, int width, int height) {
    int i, j;
    RGB *t;
    RGBA *o;
    for (i = 0; i < height; i++) {
        if (y + i >= win->height) {
            break;
        }
        if (y + i < 0) {
            continue;
        }
        for (j = 0; j < width; j++) {
            if (x + j >= win->width) {
                break;
            }
            if (x + j < 0) {
                continue;
            }
            t = win->window_buf + (y + i) * win->width + x + j;
            o = img + (height - i - 1) * width + j;
            drawPointAlpha(t, *o);
        }
    }
}

void draw24Image(window *win, RGB *img, int x, int y, int width, int height) {
    int i;
    RGB *t;
    RGB *o;
    int max_line = (win->width - x) < width ? (win->width - x) : width;
    for (i = 0; i < height; i++) {
        if (y + i >= win->height) {
            break;
        }
        if (y + i < 0) {
            continue;
        }
        t = win->window_buf + (y + i) * win->width + x;
        o = img + (height - i - 1) * width;
        memmove(t, o, max_line * 3);
    }
}

void drawRect(window *win, RGB color, int x, int y, int width, int height) {
    if (x >= win->width || x + width < 0 || y >= win->height || y + height < 0
        || x < 0 || y < 0 || width < 0 || height < 0) {
        return;
    }
    int i;
    int max_line = (win->width - x) < width ? (win->width - x) : width;
    RGB *t = win->window_buf + y * win->width + x;
    for (i = 0; i < max_line; i++) {
        *(t + i) = color;
    }
    if (y + height < win->height) {
        RGB *o = win->window_buf + (y + height) * win->width + x;
        memmove(o, t, max_line * 3);
    }
    int max_height = (win->height - y) < height ? (win->height - x) : height;
    for (i = 0; i < max_height; i++) {
        *(t + i * win->width) = color;
    }
    if (x + width < win->width) {
        t = win->window_buf + y * win->width + x + win->width;
        for (i = 0; i < max_height; i++) {
            *(t + i * win->width) = color;
        }
    }
}

void drawFillRect(window *win, RGBA color, int x, int y, int width, int height) {
    if (x >= win->width || x + width < 0 || y >= win->height || y + height < 0
        || x < 0 || y < 0 || width < 0 || height < 0) {
        return;
    }
    int i, j;
    RGB *t;
    for (i = 0; i < height; i++) {
        if (y + i >= win->height) {
            break;
        }
        if (y + i < 0) {
            continue;
        }
        for (j = 0; j < width; j++) {
            if (j + x >= win->width) {
                break;
            }
            if (j + x < 0) {
                continue;
            }
            t = win->window_buf + (y + i) * win->width + (x + j);
            drawPointAlpha(t, color);
        }
    }
}

void draw24FillRect(window *win, RGB color, int x, int y, int width, int height) {
    if (x >= win->width || x + width < 0 || y >= win->height || y + height < 0
        || x < 0 || y < 0 || width < 0 || height < 0) {
        return;
    }
    int i, j;
    int max_line = (win->width - x) < width ? (win->width - x) : width;
    RGB *t, *o;
    t = win->window_buf + y * win->width + x;
    for (i = 0; i < height; i++) {
        if (y + i >= win->height) {
            break;
        }
        if (y + i < 0) {
            continue;
        }
        if (i == 0) {
            for (j = 0; j < max_line; j++) {
                *(t + j) = color;
            }
        } else {
            o = win->window_buf + (y + i) * win->width + x;
            memmove(o, t, max_line * 3);
        }
    }
}
