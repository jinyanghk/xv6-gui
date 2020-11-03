#include "types.h"
#include "user_window.h"
#include "user.h"

void emptyHandler(Widget *w, message *msg) {
    return;
}

int getInputOffsetFromMousePosition(char* str, int width, int mouse_x, int mouse_y) {

    int charPerLine = width / CHARACTER_WIDTH;
    int offset_x = 0;
    int offset_y = 0;
    int current_pos=0;
    while (*str != '\0')
    {
        if(mouse_x==offset_x && mouse_y==offset_y) {
            return current_pos;
        }
        if (*str != '\n')
        {
            offset_x += 1;
            if (offset_x > charPerLine)
            {
                offset_x = 0;
                offset_y += 1;
            }
        }
        else
        {
            offset_x = 0;
            offset_y += 1;
        }

        str++;
        current_pos++;
    }
    return 0;
}

int getMouseXFromOffset(char* str, int width, int offset) {
    //int height = w->position.ymax - w->position.ymin;
    int charPerLine = width / CHARACTER_WIDTH;
    int offset_x = 0;
    int offset_y = 0;
    int iter=0;
    while (iter<offset)
    {
        if (str[iter] != '\n')
        {
            offset_x += 1;
            if (offset_x > charPerLine)
            {
                offset_x = 0;
                offset_y += 1;
            }
        }
        else
        {
            offset_x = 0;
            offset_y += 1;
        }
        iter++;
    }
    return offset_x;
}

int getMouseYFromOffset(char* str, int width, int offset) {
    //int height = w->position.ymax - w->position.ymin;
    int charPerLine = width / CHARACTER_WIDTH;
    int offset_x = 0;
    int offset_y = 0;
    int iter=0;
    while (iter<offset)
    {
        if (str[iter] != '\n')
        {
            offset_x += 1;
            if (offset_x > charPerLine)
            {
                offset_x = 0;
                offset_y += 1;
            }
        }
        else
        {
            offset_x = 0;
            offset_y += 1;
        }
        iter++;
    }
    return offset_y;
}

void inputFieldKeyHandler(Widget *w, message *msg)
{
    if(msg->msg_type!=M_KEY_DOWN) return;
    //int width = w->position.xmax - w->position.xmin;
    //int height = w->position.ymax - w->position.ymin;
    //int charPerLine = width / CHARACTER_WIDTH;
    int charCount = strlen(w->context.inputfield->text);
    //if(charCount>500) return;
    int newChar = msg->params[0];
    //printf(1, "new Char %d\n", newChar);
    if ((newChar >= ' ' && newChar <= '~'))
    {
        char temp[MAX_LONG_STRLEN];
        strcpy(temp, w->context.inputfield->text + w->context.inputfield->current_pos);
        w->context.inputfield->text[w->context.inputfield->current_pos++] = newChar;
        strcpy(w->context.inputfield->text + w->context.inputfield->current_pos, temp);
        //w->context.inputfield->text[charCount+1] = '\0';
        //w->context.inputfield->current_pos=charCount;
    }
    if (newChar == KEY_LF && w->context.inputfield->current_pos > 0)
    {
        w->context.inputfield->current_pos--;
    }
    if (newChar == KEY_RT && w->context.inputfield->current_pos < charCount)
    {
        w->context.inputfield->current_pos++;
    }
    /*
    if (newChar == KEY_UP && w->context.inputfield->current_pos > charPerLine)
    {
        w->context.inputfield->current_pos -= charPerLine;
    }
    if (newChar == KEY_DN && w->context.inputfield->current_pos < (charCount / charPerLine) * charPerLine)
    {
        w->context.inputfield->current_pos += charPerLine;
    }
    */
    if (newChar == '\b' && w->context.inputfield->current_pos > 0)
    {
        char temp[MAX_LONG_STRLEN];
        strcpy(temp, w->context.inputfield->text + w->context.inputfield->current_pos);
        strcpy(w->context.inputfield->text + w->context.inputfield->current_pos - 1, temp);
        w->context.inputfield->current_pos--;
    }
}