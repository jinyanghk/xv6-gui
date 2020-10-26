#include "types.h"
#include "x86.h"
#include "param.h"
#include "defs.h"
#include "msg.h"
#include "spinlock.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

#include "user_gui.h"

#include "window_manager.h"

#define MAX_WIDTH 800
#define MAX_HEIGHT 600

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
static msg_buf globalMessages;

#define MAXWINDOW 20

static window_p windowHead;
static window_p windowTail;

static int clickedOnTitle, clickedOnContent;

int next_window_id = 1;

struct spinlock wmlock;

static struct
{
	float x, y;
} wm_mouse_pos, wm_last_mouse_pos;

#define MOUSE_SPEED_X 1.0f
#define MOUSE_SPEED_Y -1.0f;

int isInRect(int xmin, int ymin, int xmax, int ymax, int x, int y)
{
	return (x >= xmin && x <= xmax && y >= ymin && y <= ymax);
}

int dispatchMessage(msg_buf *buf, message *msg)
{
	if (buf->cnt >= MSG_BUF_SIZE) return 1;
	++buf->cnt;
	buf->data[buf->rear] = *msg;
	if ((++buf->rear) >= MSG_BUF_SIZE) buf->rear = 0;
	return 0;
}

//return non-zero if buf is empty
int getMessage(msg_buf *buf, message *result)
{
	if (buf->cnt == 0) return 1;
	--buf->cnt;
	*result = buf->data[buf->front];
	if ((++buf->front) >= MSG_BUF_SIZE) buf->front = 0;
	return 0;
}

void initMessageQueue(msg_buf *buf)
{
	buf->front = buf->rear = buf->cnt = 0;
}

void wmInit()
{
	wm_mouse_pos.x = SCREEN_WIDTH / 2;
	wm_mouse_pos.y = SCREEN_HEIGHT / 2;
	wm_last_mouse_pos = wm_mouse_pos;

	windowHead=0;
	windowTail=0;

	clickedOnTitle = clickedOnContent = 0;

	initMessageQueue(&globalMessages);

	initlock(&wmlock, "wmlock");
}

void focusWindow(window_p focus)
{
	window_p p;
	for (p = windowHead; p != 0; p = p->next)
	{
		if (p == focus)
		{
			window_p prevWindow = p->prev;
			window_p nextWindow = p->next;
			if (prevWindow != 0)
			{
				prevWindow->next = nextWindow;
			}
			else
			{
				windowHead = nextWindow;
			}
			if (nextWindow != 0)
			{
				nextWindow->prev = prevWindow;
			}
			break;
		}
	}
	windowTail->next = focus;
	focus->prev = windowTail;
	focus->next = 0;
	windowTail = focus;
}

void moveRect(win_rect *rect, int dx, int dy)
{
	rect->xmin += dx;
	rect->xmax += dx;
	rect->ymin += dy;
	rect->ymax += dy;
}

void moveFocusWindow(int dx, int dy)
{
	moveRect(&windowTail->position, dx, dy);
}

void debugPrintWindowList()
{
	window_p p;
	cprintf("############################\n");
	if (windowHead != 0)
	{
		cprintf("current Head at %x\n", windowHead);
		cprintf("current Tail at %x\n", windowTail);
		cprintf("\n");
		
		for (p = windowHead; p != 0; p = p->next)
		{
			cprintf("current Window at %x\n", p);
			cprintf("prev Window at %x\n", p->prev);
			cprintf("next Window at %x\n", p->next);
			cprintf("current Window width %d\n", p->position.xmax - p->position.xmin);
			cprintf("\n");
		}
		
	}
}

void wmHandleMessage(message *msg)
{
	acquire(&wmlock);
	//debugPrintWindowList();
	//memset(screen, SCREEN_WIDTH * SCREEN_HEIGHT * 3, 0);
	//memset(screen_buf1, SCREEN_WIDTH * SCREEN_HEIGHT * 3, 0);
	//message newmsg;
	switch (msg->msg_type)
	{
	case M_MOUSE_MOVE:
		wm_last_mouse_pos = wm_mouse_pos;
		wm_mouse_pos.x += msg->params[0] * MOUSE_SPEED_X;
		wm_mouse_pos.y += msg->params[1] * MOUSE_SPEED_Y;
		if (wm_mouse_pos.x > SCREEN_WIDTH)
			wm_mouse_pos.x = SCREEN_WIDTH;
		if (wm_mouse_pos.y > SCREEN_HEIGHT)
			wm_mouse_pos.y = SCREEN_HEIGHT;
		if (wm_mouse_pos.x < 0)
			wm_mouse_pos.x = 0;
		if (wm_mouse_pos.y < 0)
			wm_mouse_pos.y = 0;
		//redraw mouse cursor
		//clearMouse(screen, screen_buf1, wm_last_mouse_pos.x, wm_last_mouse_pos.y);
		//drawMouse(screen, 0, wm_mouse_pos.x, wm_mouse_pos.y);
		//drag

		if (clickedOnTitle)
		{
			//moveFocusWindow(wm_mouse_pos.x - wm_last_mouse_pos.x, wm_mouse_pos.y - wm_last_mouse_pos.y);
		}
		break;
	case M_MOUSE_DOWN:
		//handle focus changes
		
		if (windowTail != 0 && !isInRect(windowTail->position.xmin, windowTail->position.ymin - TITLE_HEIGHT, windowTail->position.xmax, windowTail->position.ymax, wm_mouse_pos.x, wm_mouse_pos.y))
		{
			//dispatchMessage(&globalMessages, msg);	
			/*
			window_p p;
			for (p = windowHead; p != 0; p = p->next)
			{

				if (isInRect(p->position.xmin, p->position.ymin - TITLE_HEIGHT, p->position.xmax, p->position.ymax, wm_mouse_pos.x, wm_mouse_pos.y))
				{
					focusWindow(p);
					break;
				}
			}
			*/
		}
		
		/*
		if (windowTail != 0 && isInRect(windowTail->position.xmin, windowTail->position.ymin, windowTail->position.xmax, windowTail->position.ymax, wm_mouse_pos.x, wm_mouse_pos.y))
		{
			clickedOnContent = 1;
			//newmsg = *msg;
			//coordinate transformation (from screen to window)
			//newmsg.params[0] = wm_mouse_pos.x - windowlist[focus].wnd.contents.xmin;
			//newmsg.params[1] = wm_mouse_pos.y - windowlist[focus].wnd.contents.ymin;
			//newmsg.params[2] = msg->params[0];
			//dispatchMessage(focus, &newmsg);
		}
		//else if (p!=0 && wm_mouse_pos.x + 30 > p->position.xmax) //close
		//{
		//newmsg.msg_type = WM_WINDOW_CLOSE;
		//dispatchMessage(focus, &newmsg);
		//}
		else // titlebar
		{
			clickedOnTitle = 1;
		}
		*/

		break;
	case M_MOUSE_LEFT_CLICK:
		/*
		if (clickedOnContent)
		{
			clickedOnContent = 0;
			
			newmsg = *msg;
			newmsg.params[0] = wm_mouse_pos.x - windowlist[focus].wnd.contents.xmin;
			newmsg.params[1] = wm_mouse_pos.y - windowlist[focus].wnd.contents.ymin;
			newmsg.params[2] = msg->params[0];
			dispatchMessage(focus, &newmsg);
			
		}
		*/
		break;
	case M_MOUSE_RIGHT_CLICK:
		/*
		if (clickedOnContent)
		{
			
			clickedOnContent = 0;
			newmsg = *msg;
			newmsg.params[0] = wm_mouse_pos.x - windowlist[focus].wnd.contents.xmin;
			newmsg.params[1] = wm_mouse_pos.y - windowlist[focus].wnd.contents.ymin;
			newmsg.params[2] = msg->params[0];
			dispatchMessage(focus, &newmsg);
			
		}
		*/
		break;
	case M_MOUSE_DBCLICK:
		/*
		if (clickedOnContent)
		{
			
			clickedOnContent = 0;
			newmsg = *msg;
			newmsg.params[0] = wm_mouse_pos.x - windowlist[focus].wnd.contents.xmin;
			newmsg.params[1] = wm_mouse_pos.y - windowlist[focus].wnd.contents.ymin;
			newmsg.params[2] = msg->params[0];
			dispatchMessage(focus, &newmsg);
			
		}
		*/
		break;
	case M_MOUSE_UP:
		/*
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
		*/
		break;
	case M_KEY_DOWN:
		//dispatchMessage(focus, msg);
		break;
	case M_KEY_UP:
		//dispatchMessage(focus, msg);
		break;

	default:
		break;
	}
	//memmove(screen, screen_buf1, SCREEN_WIDTH * SCREEN_HEIGHT * 3);
	release(&wmlock);
}

void updateScreen()
{

	acquire(&wmlock);
	//debugPrintWindowList();

	message msg;
	if(getMessage(&globalMessages, &msg)==0) {
		cprintf("message get\n");
	}
	memset(screen_buf1, 255, screen_size);
	if (windowHead != 0)
	{
		window_p p = windowHead;
		int width = p->position.xmax - p->position.xmin;
		int height = p->position.ymax - p->position.ymin;
		draw24ImagePart(screen_buf1, p->window_buf, p->position.xmin, p->position.ymin,
							width, height, 0, 0, width, height);
		while (p->next!=0)
		{
			p=p->next;
			int width = p->position.xmax - p->position.xmin;
			int height = p->position.ymax - p->position.ymin;
			draw24ImagePart(screen_buf1, p->window_buf, p->position.xmin, p->position.ymin,
							width, height, 0, 0, width, height);
			draw24ImagePart(screen_buf1, p->title_buf, p->position.xmin, p->position.ymin - TITLE_HEIGHT,
							width, TITLE_HEIGHT, 0, 0, width, TITLE_HEIGHT);
		} 
	}

	drawMouse(screen_buf1, 0, wm_mouse_pos.x, wm_mouse_pos.y);
	memmove(screen, screen_buf1, screen_size);

	release(&wmlock);
}

//return window handler on succuss, -1 if unsuccessful
int createWindow(window_p window)
{

	acquire(&wmlock);

	if (windowHead == 0)
	{
		windowHead = window;
	}
	if (windowTail != 0)
	{
		windowTail->next = window;
	}

	if (windowHead == window)
	{
		window->prev = 0;
	}
	else
	{
		window->prev = windowTail;
	}

	windowTail = window;
	window->next = 0;
	//initMessageQueue(&window->messages);

	release(&wmlock);

	return 0;
}

int sys_createWindow()
{
	window *wnd;
	argptr(0, (char **)&wnd, sizeof(window));
	return createWindow(wnd);
}

int sys_updateScreen()
{
	updateScreen();
	return 0;
}
