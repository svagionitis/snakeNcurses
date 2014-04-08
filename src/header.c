#include <string.h>
#include "header.h"
#include "utils.h"

void print_header(WINDOW *win)
{
    char buf[50];
    int char_ret[7], i = 0;
    int header_width = 0;

    wclear(win);

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "Max Height: "/*, snake_p.height*/);
    color_str(win, 0, 0, 0, 0, buf);
    header_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "Max Width: "/*, snake_p.width*/);
    color_str(win, 0, ++header_width, 0, 0, buf);
    header_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "SNAKE: Max Height: "/*, snake_p.snake_height*/);
    color_str(win, 0, ++header_width, 0, 0, buf);
    header_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "Max Width: "/*, snake_p.snake_width*/);
    color_str(win, 0, ++header_width, 0, 0, buf);
    header_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "FOOD: Max Height: "/*, food_p.food_height*/);
    color_str(win, 0, ++header_width, 0, 0, buf);
    header_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "Max Width: "/*, food_p.food_width*/);
    color_str(win, 0, ++header_width, 0, 0, buf);
    header_width += char_ret[i++];

    wnoutrefresh(win);
}


