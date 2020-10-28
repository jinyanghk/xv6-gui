#ifndef __ASSEMBLER__
struct RGB;
struct RGBA;

#define MAX_WIDTH 800
#define MAX_HEIGHT 600

#define TITLE_HEIGHT 32
#define MSG_BUF_SIZE 50

#include "msg.h"

typedef struct win_rect
{
	int xmin, xmax, ymin, ymax;
} win_rect;

typedef struct msg_buf
{
	message data[MSG_BUF_SIZE];
	int front, rear, cnt;
} msg_buf;

typedef struct window {
    struct RGB *window_buf;
    int width;
    int height;
    //msg_buf messages;
} window;

typedef window* window_p;

#endif