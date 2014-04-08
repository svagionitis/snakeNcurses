#include "utils.h"

int color_str(WINDOW *win, int y, int x, short fg_color, short bg_color, const char *str)
{
    short i;
    // Search all the pair of colors
    // to match with the given one.
    // Then apply the specific pair.
    // Naive way
    for (i = 0;i < COLOR_PAIRS;i++)
    {
        short f, b;
        pair_content(i, &f, &b);
        if (f == fg_color && b == bg_color)
            break;
    }

    wattron(win, COLOR_PAIR(i));

    mvwaddstr(win, y, x, str);

    wattroff(win, COLOR_PAIR(i));
    return 0;
}

