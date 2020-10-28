#ifndef __WINDOW_MANAGER_H__
#define __WINDOW_MANAGER_H__

#define MSG_BUF_SIZE 50
#define MAX_TITLE_LEN 50

/*
#include "msg.h"

typedef struct msg_buf
{
	message data[MSG_BUF_SIZE];
	int front, rear, cnt;
} msg_buf;
*/

typedef struct windowtemp
{
	win_rect position;
	//int width;
	//int height;
	//win_rect titlebar;
	//msg_buf msg_buf;
	struct RGB *window_buf;
	//struct RGB *title_buf;
	//char title[MAX_TITLE_LEN];
} windowtemp;

#endif