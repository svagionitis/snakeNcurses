/*Build: gcc -std=c99 -o snake snake.c -lncurses*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>

#define HEADER_ROWS 1
#define FOOTER_ROWS 1

#define MAX_SNAKE_LENGTH 50

int color_str(int y, int x, short fg_color, short bg_color, const char * str)
{
    short i;
    // Search all the pair of colors
    // to match with the given one.
    // Then apply the specific pair.
    // Naive way
    for (i = 1;i < COLOR_PAIRS;i++)
    {
        short f, b;
        pair_content(i, &f, &b);
        if (f == fg_color && b == bg_color)
            break;
    }

    attron(COLOR_PAIR(i));

    mvaddstr(y,x,str);

    attroff(COLOR_PAIR(i));
    return 0;
}

int print_header(int maxY, int maxX)
{
    char buf[50];
    int header_width = 0;

    memset(buf, '\0', sizeof buf);
    int char_ret1 = snprintf(buf, sizeof buf, "Max Height: %d", maxY);
    mvaddstr(0, 0, buf);
    header_width += char_ret1;

    memset(buf, '\0', sizeof buf);
    int char_ret2 = snprintf(buf, sizeof buf, "Max Width: %d", maxX);
    mvaddstr(0, ++header_width, buf);
    header_width += char_ret2;

    refresh();
    return 0;
}

int print_footer(int maxY, int x, int y, int time)
{
    char buf[50];
    int footer_width = 0;

    memset(buf, '\0', sizeof buf);
    int char_ret1 = snprintf(buf, sizeof buf, "x: %d", x);
    mvaddstr(maxY - 1, 0, buf);
    footer_width += char_ret1;

    memset(buf, '\0', sizeof buf);
    int char_ret2 = snprintf(buf, sizeof buf, "y: %d", y);
    mvaddstr(maxY - 1, ++footer_width, buf);
    footer_width += char_ret2;

    memset(buf, '\0', sizeof buf);
    int char_ret3 = snprintf(buf, sizeof buf, "time: %d", time);
    mvaddstr(maxY - 1, ++footer_width, buf);
    footer_width += char_ret3;

    refresh();
    return 0;
}

int print_snake(int time, int x[], int y[], int length)
{
    for (int i = 0;i<length;i++)
    {
        if (time - i >= 0)
        {
            if (i == 0)
                color_str(y[time-i], x[time-i], -1, -1, "*");
            else
                color_str(y[time-i], x[time-i], -1, -1, "@");
        }
        else
            color_str(y[MAX_SNAKE_LENGTH-i+time], x[MAX_SNAKE_LENGTH-i+time], -1, -1, "@");
    }

    refresh();
    return 0;
}

int main(int argc, char *argv[])
{
    WINDOW *mainwin;
    int ch;
    int maxX = 0, maxY = 0;
    int x = 5, y = 5;
    int length = 10;


    mainwin = initscr();
    if (mainwin == NULL)
    {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(-1);
    }

    // Get the maximum size of the screen
    getmaxyx(mainwin, maxY, maxX);

    // Check if colors are supported
    if (!has_colors())
    {
        delwin(mainwin);
        endwin();
        fprintf(stderr,"Your terminal does not support color\n");
        exit(-1);
    }
    else
    {
        // Initialize colors
        start_color();

        // Assign terminal default foreground/background colors to color number -1
        use_default_colors();

        // Initialize color pairs.
        short pair_count = 0;
        for(short i = 0;i<COLORS;i++)
            for(short j = 0;j<COLORS;j++)
                init_pair(pair_count++, i, j);
    }


    clear();

    // Turn off key echoing
    noecho();
    // Line buffering disabled
    cbreak();
    // This determines the visibility of the cursor.
    curs_set(FALSE);
    // Tell curses not to do NL->CR/NL on output
    nonl();
    // Enable the keypad for non-char keys
    keypad(mainwin, TRUE);

    int time = 0, _x[MAX_SNAKE_LENGTH], _y[MAX_SNAKE_LENGTH];
    memset(_x, 0, sizeof _x);
    memset(_y, 0, sizeof _y);

    // Print tree and then wait for a key
    print_header(maxY, maxX);
    print_snake(time, _x, _y, length);
    print_footer(maxY, x, y, time);

    // Loop until press q
    while ((ch = getch()) != 'q')
    {

        switch(ch)
        {
            case '0': // Set foreground/background colors to default
            case '1': // Set foreground/background colors according to init_pairs
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                attrset(COLOR_PAIR(ch - '0'));
                break;
            case KEY_UP:
                y -= 1;

                if (y < 0 + HEADER_ROWS)
                    y = maxY - FOOTER_ROWS;

                time++;
                if (time >= MAX_SNAKE_LENGTH)
                    time = 0;

                _x[time] = x;
                _y[time] = y;
                break;
            case KEY_DOWN:
                y += 1;

                if ( y > maxY)
                    y = 0 + HEADER_ROWS;

                time++;
                if (time >= MAX_SNAKE_LENGTH)
                    time = 0;

                _x[time] = x;
                _y[time] = y;
                break;
            case KEY_RIGHT:
                x += 1;

                if (x > maxX)
                    x = 0;

                time++;
                if (time >= MAX_SNAKE_LENGTH)
                    time = 0;

                _x[time] = x;
                _y[time] = y;
                break;
            case KEY_LEFT:
                x -= 1;

                if (x < 0)
                    x = maxX;

                time++;
                if (time >= MAX_SNAKE_LENGTH)
                    time = 0;

                _x[time] = x;
                _y[time] = y;
                break;
        }

        erase();
        print_header(maxY, maxX);

        print_snake(time, _x, _y, length);

        print_footer(maxY, x, y, time);
    }


    delwin(mainwin);
    endwin();
    refresh();

    return 0;
}

