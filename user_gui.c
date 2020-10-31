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

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void drawButtonWidget(window *win, Widget *w);

int isInRect(int xmin, int ymin, int xmax, int ymax, int x, int y)
{
    return (x >= xmin && x <= xmax && y >= ymin && y <= ymax);
}

void createPopupWindow(window *win, int caller)
{

    int width = win->width;
    int height = win->height;

    win->window_buf = malloc(width * height * 3);
    if (!win->window_buf)
    {
        return;
    }
    memset(win->window_buf, 255, height * width * 3);
    win->widgetlisthead = -1;
    win->widgetlisttail = -1;
    int i;
    for (i = 0; i < MAX_WIDGET_SIZE; ++i)
    {
        win->widgets[i].next = i;
        win->widgets[i].prev = i;
    }
    win->needsRepaint = 1;
    win->hasTitleBar = 0;
    //initlock(&win->wmlock, "wmlock");
    GUI_createPopupWindow(win, caller);
}

void closePopupWindow(window *win)
{
    free(win->window_buf);
    GUI_closePopupWindow(win);
    exit();
}

void createWindow(window *win, const char *title)
{

    int width = win->width;
    int height = win->height;

    win->window_buf = malloc(width * height * 3);
    if (!win->window_buf)
    {
        return;
    }
    memset(win->window_buf, 255, height * width * 3);
    win->widgetlisthead = -1;
    win->widgetlisttail = -1;
    int i;
    for (i = 0; i < MAX_WIDGET_SIZE; ++i)
    {
        win->widgets[i].next = i;
        win->widgets[i].prev = i;
    }
    win->needsRepaint = 1;
    if (win->hasTitleBar != 0)
    {
        win->hasTitleBar = 1;
    }
    //initlock(&win->wmlock, "wmlock");
    GUI_createWindow(win, title);
}

void closeWindow(window *win)
{
    free(win->window_buf);
    GUI_closeWindow(win);
    exit();
}

void updateWindow(window *win)
{

    if (win->needsRepaint)
    {
        for (int p = win->widgetlisthead; p != -1; p = win->widgets[p].next)
        {
            drawButtonWidget(win, &win->widgets[p]);
        }
    }

    message msg;
    if (GUI_getMessage(win->handler, &msg) == 0)
    {
        win->needsRepaint = 1;
        //TODO: check widgets to determine
        if (msg.msg_type != 0)
        {
            printf(1, "message is %d\n", msg.msg_type);
            printf(1, "mouse at %d, %d\n", msg.params[0], msg.params[1]);
        }
        if (msg.msg_type == WM_WINDOW_CLOSE)
        {
            closeWindow(win);
        }
        else if (msg.msg_type == WM_WINDOW_MINIMIZE)
        {
            GUI_minimizeWindow(win);
        }
        else if (msg.msg_type == WM_WINDOW_MAXIMIZE)
        {
            GUI_maximizeWindow(win);
        }
        else
        {
            int mouse_x = msg.params[0];
            int mouse_y = msg.params[1];

            for (int p = win->widgetlisttail; p != -1; p = win->widgets[p].prev)
            {
                if (isInRect(win->widgets[p].position.xmin, win->widgets[p].position.ymin, win->widgets[p].position.xmax, win->widgets[p].position.ymax, mouse_x, mouse_y))
                {
                    win->widgets[p].handler(&msg);
                    break;
                }
            }
        }
    }
    else
    {
        win->needsRepaint = 0;
    }
    return;
}

void updatePopupWindow(window *win)
{
    if (win->needsRepaint)
    {
        for (int p = win->widgetlisthead; p != -1; p = win->widgets[p].next)
        {
            drawButtonWidget(win, &win->widgets[p]);
        }
    }

    message msg;
    if (GUI_getPopupMessage(&msg) == 0)
    {
        win->needsRepaint = 1;
        if (msg.msg_type != 0)
        {
            printf(1, "message is %d\n", msg.msg_type);
            printf(1, "mouse at %d, %d\n", msg.params[0], msg.params[1]);
        }
        if (msg.msg_type == WM_WINDOW_CLOSE)
        {
            closePopupWindow(win);
        }
        else
        {
            int mouse_x = msg.params[0];
            int mouse_y = msg.params[1];
            for (int p = win->widgetlisttail; p != -1; p = win->widgets[p].prev)
            {
                if (isInRect(win->widgets[p].position.xmin, win->widgets[p].position.ymin, win->widgets[p].position.xmax, win->widgets[p].position.ymax, mouse_x, mouse_y))
                {
                    win->widgets[p].handler(&msg);
                    break;
                }
            }
        }
    }
    else
    {
        win->needsRepaint = 0;
    }
    return;
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

void drawString(window *win, int x, int y, char *str, RGBA color, int width)
{
    int offset_x = 0;

    while (*str != '\0')
    {
        if (x + offset_x >= win->width || offset_x >= width)
        { // if too long
            break;
        }
        offset_x += drawCharacter(win, x + offset_x, y, *str, color);
        str++;
    }
}

void drawImage(window *win, RGBA *img, int x, int y, int width, int height)
{
    int i, j;
    RGB *t;
    RGBA *o;
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

void setWidgetSize(Widget *widget, int x, int y, int w, int h)
{
    widget->position.xmin = x;
    widget->position.ymin = y;
    widget->position.xmax = x + w;
    widget->position.ymax = y + h;
}

int findNextAvailable(window *win)
{

    for (int i = 0; i < MAX_WIDGET_SIZE; i++)
    {
        if (win->widgets[i].prev == i && win->widgets[i].next == i)
        {
            return i;
        }
    }
    return -1;
}

void addToWidgetListTail(window *win, int idx)
{
    win->widgets[idx].prev = win->widgetlisttail;
    win->widgets[idx].next = -1;
    if (win->widgetlisttail != -1)
        win->widgets[win->widgetlisttail].next = idx;
    win->widgetlisttail = idx;
}

void removeFromWidgetList(window *win, int idx)
{
    if (win->widgetlisttail == idx)
        win->widgetlisttail = win->widgets[win->widgetlisttail].prev;
    if (win->widgets[idx].prev != -1)
        win->widgets[win->widgets[idx].prev].next = win->widgets[idx].next;
    if (win->widgets[idx].next != -1)
        win->widgets[win->widgets[idx].next].prev = win->widgets[idx].prev;
}

int addButtonWidget(window *win, RGBA c, RGBA bc, char *text, int x, int y, int w, int h, Handler handler)
{

    int widgetId = findNextAvailable(win);
    if (widgetId == -1)
        return 1;

    if (win->widgetlisthead == -1)
    {
        win->widgetlisthead = widgetId;
    }

    addToWidgetListTail(win, widgetId);

    Button *b = malloc(sizeof(Button));
    b->bg_color = bc;
    b->color = c;
    strcpy(b->text, text);

    Widget *widget = &win->widgets[widgetId];
    widget->context.button = b;
    widget->type = BUTTON;
    widget->handler = handler;
    setWidgetSize(widget, x, y, w, h);

    return 0;
}

void drawButtonWidget(window *win, Widget *w)
{
    RGB black;
    black.R = 0;
    black.G = 0;
    black.B = 0;
    int width = w->position.xmax - w->position.xmin;
    int height = w->position.ymax - w->position.ymin;
    drawFillRect(win, w->context.button->bg_color, w->position.xmin, w->position.ymin, width, height);
    drawRect(win, black, w->position.xmin, w->position.ymin, width, height);
    drawString(win, w->position.xmin, w->position.ymin, w->context.button->text, w->context.button->color, width);
}
