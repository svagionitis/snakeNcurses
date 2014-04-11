#include <string.h>
#include "header.h"
#include "utils.h"
#include "snake.h"
#include "food.h"

extern snake_param_t snake_p;
extern food_param_t food_p;

void print_header(WINDOW *win)
{
    char buf[50];
    int char_ret[7], i = 0;
    int header_width = 0;

    wclear(win);

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "Max Height: %d", snake_p.height);
    color_str(win, 0, 0, 0, 0, buf);
    header_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "Max Width: %d", snake_p.width);
    color_str(win, 0, ++header_width, 0, 0, buf);
    header_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "SNAKE: Max Height: %d", snake_p.snake_height);
    color_str(win, 0, ++header_width, 0, 0, buf);
    header_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "Max Width: %d", snake_p.snake_width);
    color_str(win, 0, ++header_width, 0, 0, buf);
    header_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "FOOD: Max Height: %d", food_p.food_height);
    color_str(win, 0, ++header_width, 0, 0, buf);
    header_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "Max Width: %d", food_p.food_width);
    color_str(win, 0, ++header_width, 0, 0, buf);
    header_width += char_ret[i++];

    wnoutrefresh(win);
}


