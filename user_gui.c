#include "types.h"
#include "stat.h"
//#include "color.h"
#include "msg.h"
#include "user.h"
#include "fcntl.h"
#include "user_window.h"
#include "character.h"
#include "icons.h"
#include "fs.h"
#include "gui.h"

int min(int x, int y) { return x < y ? x : y; }
int max(int x, int y) { return x > y ? x : y; }

void drawPoint(RGB *color, RGB origin)
{
    color->R = origin.R;
    color->G = origin.G;
    color->B = origin.B;
}

void drawPointAlpha(RGB *color, RGBA origin)
{
    float alpha;
    if (origin.A == 255)
    {
        color->R = origin.R;
        color->G = origin.G;
        color->B = origin.B;
        return;
    }
    if (origin.A == 0)
    {
        return;
    }
    alpha = (float)origin.A / 255;
    color->R = color->R * (1 - alpha) + origin.R * alpha;
    color->G = color->G * (1 - alpha) + origin.G * alpha;
    color->B = color->B * (1 - alpha) + origin.B * alpha;
}

void fillRect(RGB *buf, int x, int y, int width, int height, int max_x, int max_y, RGBA fill)
{
    int i, j;
    RGB *t;
    for (i = 0; i < height; i++)
    {
        if (y + i < 0)
            continue;
        if (y + i >= max_y)
            break;
        for (j = 0; j < width; j++)
        {
            if (x + j < 0)
                continue;
            if (x + j >= max_x)
                break;
            t = buf + (y + i) * max_x + x + j;
            drawPointAlpha(t, fill);
        }
    }
}

int drawCharacter(window *win, int x, int y, char ch, RGBA color)
{
    int i, j;
    RGB *t;
    int ord = ch - 0x20;
    if (ord < 0 || ord >= (CHARACTER_NUMBER - 1))
    {
        return -1;
    }
    for (i = 0; i < CHARACTER_HEIGHT; i++)
    {
        if (y + i > win->height)
        {
            break;
        }
        if (y + i < 0)
        {
            continue;
        }
        for (j = 0; j < CHARACTER_WIDTH; j++)
        {
            if (character[ord][i][j] == 1)
            {
                if (x + j > win->width)
                {
                    break;
                }
                if (x + j < 0)
                {
                    continue;
                }
                t = win->window_buf + (y + i) * win->width + x + j;
                drawPointAlpha(t, color);
            }
        }
    }
    return CHARACTER_WIDTH;
}

void drawString(window *win, char *str, RGBA color, int x, int y, int width, int height)
{
    int offset_x = 0;
    int offset_y = 0;
    //int charPerLine = width / CHARACTER_WIDTH;
    //y = y - win->scrollOffset;

    while (*str != '\0')
    {
        if (offset_y > height)
            break;
        if (*str != '\n')
        {
            if (x + offset_x >= 0 && x + offset_x <= win->width && y + offset_y >= 0 && y + offset_y <= win->height)
            {
                drawCharacter(win, x + offset_x, y + offset_y, *str, color);
            }

            offset_x += CHARACTER_WIDTH;
            if (offset_x > width)
            {
                offset_x = 0;
                offset_y += CHARACTER_HEIGHT;
            }
        }
        else
        {
            offset_x = 0;
            offset_y += CHARACTER_HEIGHT;
        }

        str++;
    }
}

void drawImage(window *win, RGBA *img, int x, int y, int width, int height)
{
    int i, j;
    RGB *t;
    RGBA *o;
    //y = y - win->scrollOffset;
    for (i = 0; i < height; i++)
    {
        if (y + i >= win->height)
        {
            break;
        }
        if (y + i < 0)
        {
            continue;
        }
        for (j = 0; j < width; j++)
        {
            if (x + j >= win->width)
            {
                break;
            }
            if (x + j < 0)
            {
                continue;
            }
            t = win->window_buf + (y + i) * win->width + x + j;
            o = img + (height - i - 1) * width + j;
            drawPointAlpha(t, *o);
        }
    }
}

void draw24Image(window *win, RGB *img, int x, int y, int width, int height)
{
    //y = y - win->scrollOffset;
    int i;
    RGB *t;
    RGB *o;
    int max_line = (win->width - x) < width ? (win->width - x) : width;
    for (i = 0; i < height; i++)
    {
        if (y + i >= win->height)
        {
            break;
        }
        if (y + i < 0)
        {
            continue;
        }
        t = win->window_buf + (y + i) * win->width + x;
        o = img + (height - i - 1) * width;
        memmove(t, o, max_line * 3);
    }
}

void drawRect(window *win, RGB color, int x, int y, int width, int height)
{
    //y = y - win->scrollOffset;
    if (x >= win->width || x + width < 0 || y >= win->height || y + height < 0 || x < 0 || y < 0 || width < 0 || height < 0)
    {
        return;
    }
    int i;
    int max_line = (win->width - x) < width ? (win->width - x) : width;
    RGB *t = win->window_buf + y * win->width + x;
    for (i = 0; i < max_line; i++)
    {
        *(t + i) = color;
    }
    if (y + height < win->height)
    {
        RGB *o = win->window_buf + (y + height) * win->width + x;
        memmove(o, t, max_line * 3);
    }
    int max_height = (win->height - y) < height ? (win->height - x) : height;
    for (i = 0; i < max_height; i++)
    {
        *(t + i * win->width) = color;
    }
    if (x + width < win->width)
    {
        t = win->window_buf + y * win->width + x + win->width;
        for (i = 0; i < max_height; i++)
        {
            *(t + i * win->width) = color;
        }
    }
}

void drawFillRect(window *win, RGBA color, int x, int y, int width, int height)
{
   // y = y - win->scrollOffset;
    if (x >= win->width || x + width < 0 || y >= win->height || y + height < 0 || x < 0 || y < 0 || width < 0 || height < 0)
    {
        return;
    }
    int i, j;
    RGB *t;
    for (i = 0; i < height; i++)
    {
        if (y + i >= win->height)
        {
            break;
        }
        if (y + i < 0)
        {
            continue;
        }
        for (j = 0; j < width; j++)
        {
            if (j + x >= win->width)
            {
                break;
            }
            if (j + x < 0)
            {
                continue;
            }
            t = win->window_buf + (y + i) * win->width + (x + j);
            drawPointAlpha(t, color);
        }
    }
}

void draw24FillRect(window *win, RGB color, int x, int y, int width, int height)
{
    //y = y - win->scrollOffset;
    if (x >= win->width || x + width < 0 || y >= win->height || y + height < 0 || x < 0 || y < 0 || width < 0 || height < 0)
    {
        return;
    }
    int i, j;
    int max_line = (win->width - x) < width ? (win->width - x) : width;
    RGB *t, *o;
    t = win->window_buf + y * win->width + x;
    for (i = 0; i < height; i++)
    {
        if (y + i >= win->height)
        {
            break;
        }
        if (y + i < 0)
        {
            continue;
        }
        if (i == 0)
        {
            for (j = 0; j < max_line; j++)
            {
                *(t + j) = color;
            }
        }
        else
        {
            o = win->window_buf + (y + i) * win->width + x;
            memmove(o, t, max_line * 3);
        }
    }
}

void drawIcon(window *win, int icon, RGBA color, int x, int y, int width, int height)
{
    //y = y - win->scrollOffset;
    int i, j;
    RGB *t;
    if (icon < 0 || icon >= (ICON_NUMBER - 1))
    {
        return;
    }
    for (i = 0; i < min(ICON_SIZE, width); i++)
    {
        if (y + i > win->height)
        {
            break;
        }
        if (y + i < 0)
        {
            continue;
        }
        for (j = 0; j < min(ICON_SIZE, width); j++)
        {
            if (icons[icon][i][j] == 1)
            {
                if (x + j > win->width)
                {
                    break;
                }
                if (x + j < 0)
                {
                    continue;
                }
                t = win->window_buf + (y + i) * win->width + x + j;
                drawPointAlpha(t, color);
            }
        }
    }
    return;
}
