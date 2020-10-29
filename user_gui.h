#ifndef __ASSEMBLER__
struct RGB;
struct RGBA;

#define MAX_WIDTH 800
#define MAX_HEIGHT 600



typedef struct window {
    struct RGB *window_buf;
    int width;
    int height;
    int handler;
    //msg_buf messages;
} window;

typedef window* window_p;

#endif