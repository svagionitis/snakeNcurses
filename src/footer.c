#include <string.h>
#include "footer.h"
#include "utils.h"

void print_footer(WINDOW *win)
{
    char buf[50];
    int char_ret[9], i = 0;
    int footer_width = 0;

    wclear(win);

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "FOOD:");
    color_str(win, 0, 0, 0, 0, buf);
    footer_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "x: "/*, food_p.x*/);
    color_str(win, 0, ++footer_width, 0, 0, buf);
    footer_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "y: "/*, food_p.y*/);
    color_str(win, 0, ++footer_width, 0, 0, buf);
    footer_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "SNAKE:");
    color_str(win, 0, ++footer_width, 0, 0, buf);
    footer_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "x: "/*, snake_p.x*/);
    color_str(win, 0, ++footer_width, 0, 0, buf);
    footer_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "y: "/*, snake_p.y*/);
    color_str(win, 0, ++footer_width, 0, 0, buf);
    footer_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "moves: "/*, snake_p.moves*/);
    color_str(win, 0, ++footer_width, 0, 0, buf);
    footer_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "speed: "/*, snake_p.speed*/);
    color_str(win, 0, ++footer_width, 0, 0, buf);
    footer_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "length: "/*, snake_p.length*/);
    color_str(win, 0, ++footer_width, 0, 0, buf);
    footer_width += char_ret[i++];

    wnoutrefresh(win);
}

