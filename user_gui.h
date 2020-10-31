#ifndef __ASSEMBLER__

#define MAX_WIDTH 800
#define MAX_HEIGHT 600

#define MAX_WIDGET_SIZE 50
#define MAX_SHORT_STRLEN 50

#include "gui.h"
#include "window_manager.h"

struct spinlock;
// @para: window widget_index message
typedef void(*Handler)(message *msg);

// @para: window widget_index
//typedef void(*painter)(struct window *win);

#define IMAGE 0
#define LABEL 1
#define BUTTON 2
#define INPUT 3
#define TEXT_AREA 4
#define FILE_LIST 5

typedef struct Button {
    struct RGBA color;
    struct RGBA bg_color;
    char text[MAX_SHORT_STRLEN];
} Button;

typedef union widget_base {
    Button *button;
} widget_base;

typedef struct Widget {
    widget_base context;
    int type;
    win_rect position;
    int next, prev;
    Handler handler;
} Widget;

typedef struct window {
    win_rect initialPosition;
    int hasTitleBar;
    struct RGB *window_buf;
    int width;
    int height;
    int handler;
    struct Widget widgets[MAX_WIDGET_SIZE];
    int widgetlisthead, widgetlisttail;
    int needsRepaint;
} window;

typedef window* window_p;

#endif