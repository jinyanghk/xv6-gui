#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "spinlock.h"
#include "gui.h"
#include "character.h"

struct spinlock screen_lock;
struct spinlock buf1_lock;
struct spinlock buf2_lock;

static RGB *screen;
static RGB screen_buf1[480000];
//static RGB *screen_buf2[480000];


void initGUI() {
    uint GraphicMem = KERNBASE + 0x1028;
    
    uint baseAdd = *((uint *) GraphicMem);
    SCREEN_PHYSADDR = (unsigned short*)baseAdd;
    VESA_ADDR = SCREEN_PHYSADDR;
    SCREEN_WIDTH = *((ushort *) (KERNBASE + 0x1012));
    SCREEN_HEIGHT = *((ushort *) (KERNBASE + 0x1014));
    screen_size = (SCREEN_WIDTH * SCREEN_HEIGHT) * 3;

    screen= (RGB*) VESA_ADDR;
    //screen_buf1 = (RGB *) (baseAdd + screen_size);
    //screen_buf2 = (RGB *) (baseAdd + screen_size * 2);
/*
    mouse_color[0].G = 0;
    mouse_color[0].B = 0;
    mouse_color[0].R = 0;
    mouse_color[1].G = 200;
    mouse_color[1].B = 200;
    mouse_color[1].R = 200;
*/
    cprintf("KERNEL BASE ADDRESS: %x\n", KERNBASE);
    cprintf("KERNEL BASE ADDRESS: %x\n", GraphicMem);
    cprintf("SCREEN PHYSICAL ADDRESS: %x\n", baseAdd);
    cprintf("SCREEN PHYSICAL ADDRESS: %x\n", screen);
    cprintf("@Screen Width:   %d\n", SCREEN_WIDTH);
    cprintf("@Screen Height:  %d\n", SCREEN_HEIGHT);
    cprintf("@Bits per pixel: %d\n", *((uchar *) (KERNBASE + 0x1019)));
    cprintf("@Video card drivers initialized successfully.\n");

    //wmInit();
}

void drawPoint(RGB *color, RGB origin) {
    color->R = origin.R;
    color->G = origin.G;
    color->B = origin.B;
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

int drawCharacter(RGB *buf, int x, int y, char ch, RGBA color) {
    int i, j;
    RGB *t;
    int ord = ch - 0x20;
    if (ord < 0 || ord >= (CHARACTER_NUMBER - 1)) {
        return -1;
    }
    for (i = 0; i < CHARACTER_HEIGHT; i++) {
        if (y + i > SCREEN_HEIGHT || y + i < 0) {
            break;
        }
        for (j = 0; j < CHARACTER_WIDTH; j++) {
            if (character[ord][i][j] == 1) {
                if (x + j > SCREEN_WIDTH || x + j < 0) {
                    break;
                }
                t = buf + (y + i) * SCREEN_WIDTH + x + j;
                drawPointAlpha(t, color);
            }
        }
    }
    return CHARACTER_WIDTH;
}

void drawString(RGB *buf, int x, int y, char *str, RGBA color) {
    int offset_x = 0;

    while (*str != '\0') {
        cprintf("writting:   %d\n", offset_x);
        offset_x += drawCharacter(buf, x + offset_x, y, *str, color);
        str++;
    }
}

int sys_drawline(void) {
    struct RGBA color;
    color.R = 170; color.G = 150; color.B = 100; color.A = 100;
    drawString(screen_buf1, 100, 200, "hello", color);
    memmove(screen, screen_buf1, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(RGB));
    return 0;
}

