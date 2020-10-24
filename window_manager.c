#include "types.h"
#include "x86.h"
#include "param.h"
#include "defs.h"
#include "msg.h"
#include "spinlock.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

#include "window_manager.h"

int min(int x, int y) { return x < y ? x : y; }
int max(int x, int y) { return x > y ? x : y; }
int clamp(int x, int l, int r) { return min(r, max(l, x)); }
/*
static int windowlisthead, emptyhead;
static int desktopHandler = -100;
static int focus;
static int frontcnt;

static int clickedOnTitle, clickedOnContent;
*/
struct spinlock wmlock;

static struct
{
	float x, y;
} wm_mouse_pos, wm_last_mouse_pos;

#define MOUSE_SPEED_X 1.0f
#define MOUSE_SPEED_Y -1.0f;

void wmHandleMessage(message *msg)
{
	acquire(&wmlock);

	//message newmsg;
	//int p;
	switch (msg->msg_type)
	{
	case M_MOUSE_MOVE:
		wm_last_mouse_pos = wm_mouse_pos;
		wm_mouse_pos.x += msg->params[0] * MOUSE_SPEED_X;
		wm_mouse_pos.y += msg->params[1] * MOUSE_SPEED_Y;
		if (wm_mouse_pos.x > SCREEN_WIDTH) wm_mouse_pos.x = SCREEN_WIDTH;
		if (wm_mouse_pos.y > SCREEN_HEIGHT) wm_mouse_pos.y = SCREEN_HEIGHT;
		if (wm_mouse_pos.x < 0) wm_mouse_pos.x = 0;
		if (wm_mouse_pos.y < 0) wm_mouse_pos.y = 0;
		//redraw mouse cursor
		clearMouse(screen, screen_buf1, wm_last_mouse_pos.x, wm_last_mouse_pos.y);
		drawMouse(screen, 0, wm_mouse_pos.x, wm_mouse_pos.y);
		//drag
	/*	
        if (clickedOnTitle)
		{
		    wmMoveFocusWindow(wm_mouse_pos.x - wm_last_mouse_pos.x, wm_mouse_pos.y - wm_last_mouse_pos.y);
		}
		break;
	case M_MOUSE_DOWN:
		//handle focus changes
		if (frontcnt == 0)
		{
		    for (p = windowlisthead; p != -1; p = windowlist[p].next)
		    {
			    if (isInRect(&windowlist[p].wnd.titlebar, wm_mouse_pos.x, wm_mouse_pos.y) ||
			        isInRect(&windowlist[p].wnd.contents, wm_mouse_pos.x, wm_mouse_pos.y))
			    {
			        if (p != focus) focusWindow(p);
			        break;
			    }
		    }
		}
		if (isInRect(&windowlist[focus].wnd.contents, wm_mouse_pos.x, wm_mouse_pos.y))
		{
    		clickedOnContent = 1;
		    newmsg = *msg;
		    //coordinate transformation (from screen to window)
		    newmsg.params[0] = wm_mouse_pos.x - windowlist[focus].wnd.contents.xmin;
		    newmsg.params[1] = wm_mouse_pos.y - windowlist[focus].wnd.contents.ymin;
		    newmsg.params[2] = msg->params[0];
		    dispatchMessage(focus, &newmsg);
		}
		else if (wm_mouse_pos.x + 30 > windowlist[focus].wnd.titlebar.xmax) //close
		{
		    newmsg.msg_type = WM_WINDOW_CLOSE;
		    dispatchMessage(focus, &newmsg);
		} else // titlebar
		{
		    clickedOnTitle = 1;
		}
		break;
	case M_MOUSE_LEFT_CLICK:
	    if (clickedOnContent)
	    {
	        clickedOnContent = 0;
		    newmsg = *msg;
	        newmsg.params[0] = wm_mouse_pos.x - windowlist[focus].wnd.contents.xmin;
	        newmsg.params[1] = wm_mouse_pos.y - windowlist[focus].wnd.contents.ymin;
	        newmsg.params[2] = msg->params[0];
		    dispatchMessage(focus, &newmsg);
	    }
	case M_MOUSE_RIGHT_CLICK:
		if (clickedOnContent)
	    {
	        clickedOnContent = 0;
		    newmsg = *msg;
	        newmsg.params[0] = wm_mouse_pos.x - windowlist[focus].wnd.contents.xmin;
	        newmsg.params[1] = wm_mouse_pos.y - windowlist[focus].wnd.contents.ymin;
	        newmsg.params[2] = msg->params[0];
		    dispatchMessage(focus, &newmsg);
	    }
	case M_MOUSE_DBCLICK:
	    if (clickedOnContent)
	    {
	        clickedOnContent = 0;
		    newmsg = *msg;
	        newmsg.params[0] = wm_mouse_pos.x - windowlist[focus].wnd.contents.xmin;
	        newmsg.params[1] = wm_mouse_pos.y - windowlist[focus].wnd.contents.ymin;
	        newmsg.params[2] = msg->params[0];
		    dispatchMessage(focus, &newmsg);
	    }
	case M_MOUSE_UP:
		if (clickedOnContent)
		{
		    clickedOnContent = 0;
		    newmsg = *msg;
	        newmsg.params[0] = wm_mouse_pos.x - windowlist[focus].wnd.contents.xmin;
	        newmsg.params[1] = wm_mouse_pos.y - windowlist[focus].wnd.contents.ymin;
	        newmsg.params[2] = msg->params[0];
		    dispatchMessage(focus, &newmsg);
		}
		else if (clickedOnTitle)
		{
		    clickedOnTitle = 0;
		}
		break;
	case M_KEY_DOWN:
		dispatchMessage(focus, msg);
		break;
	case M_KEY_UP:
		dispatchMessage(focus, msg);
		break;
    */
	default:
		break;
	}

	release(&wmlock);
}
