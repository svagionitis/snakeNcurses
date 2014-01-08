/*Build: gcc -D_BSD_SOURCE -std=c99 -o snake snake.c -lncurses -lpthread*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <curses.h>
#include <pthread.h>

#define HEADER_ROWS 1
#define FOOTER_ROWS 1

#define MAX_SNAKE_LENGTH 50

typedef struct snake_param
{
    int ch;
    int x;
    int y;
    int move_x[MAX_SNAKE_LENGTH];
    int move_y[MAX_SNAKE_LENGTH];
    int maxX;
    int maxY;
    int moves;
    int length;
    unsigned int speed;
} snake_param_t;

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

void get_keyboard_info(void *param)
{
    snake_param_t *p = (snake_param_t *)param;
    int ch;

    while(p->ch != 'q')
    {
        int last_char;

        // Get keyboard input non-blocking
        p->ch = getch();
        if (p->ch == ERR)
            p->ch = last_char;
        else if (p->ch == 'q')
            break;

        switch(p->ch)
        {
            case KEY_UP:
                p->y -= 1;

                if (p->y < 0 + HEADER_ROWS)
                    p->y = p->maxY - FOOTER_ROWS - 1;

                p->moves++;
                if (p->moves >= MAX_SNAKE_LENGTH)
                    p->moves = 0;

                p->move_x[p->moves] = p->x;
                p->move_y[p->moves] = p->y;
                break;
            case KEY_DOWN:
                p->y += 1;

                if ( p->y >= p->maxY - FOOTER_ROWS)
                    p->y = 0 + HEADER_ROWS;

                p->moves++;
                if (p->moves >= MAX_SNAKE_LENGTH)
                    p->moves = 0;

                p->move_x[p->moves] = p->x;
                p->move_y[p->moves] = p->y;
                break;
            case KEY_RIGHT:
                p->x += 1;

                if (p->x > p->maxX)
                    p->x = 0;

                p->moves++;
                if (p->moves >= MAX_SNAKE_LENGTH)
                    p->moves = 0;

                p->move_x[p->moves] = p->x;
                p->move_y[p->moves] = p->y;
                break;
            case KEY_LEFT:
                p->x -= 1;

                if (p->x < 0)
                    p->x = p->maxX;

                p->moves++;
                if (p->moves >= MAX_SNAKE_LENGTH)
                    p->moves = 0;

                p->move_x[p->moves] = p->x;
                p->move_y[p->moves] = p->y;
                break;
            case '+':
                p->speed += 100;

                break;
            case '-':
                p->speed -= 100;

                break;
            case '*':
                p->speed *= 10;

                break;
            case '/':
                p->speed /= 10;

                if (p->speed < 1)
                    p->speed = 1;

                break;
        }

        if (p->ch == KEY_UP || p->ch == KEY_DOWN || p->ch == KEY_LEFT || p->ch == KEY_RIGHT)
            last_char = p->ch;
    }

    pthread_exit(0);
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

int print_footer(int maxY, int x, int y, int moves, unsigned int speed)
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
    int char_ret3 = snprintf(buf, sizeof buf, "moves: %d", moves);
    mvaddstr(maxY - 1, ++footer_width, buf);
    footer_width += char_ret3;

    memset(buf, '\0', sizeof buf);
    int char_ret4 = snprintf(buf, sizeof buf, "speed: %u", speed);
    mvaddstr(maxY - 1, ++footer_width, buf);
    footer_width += char_ret4;

    refresh();
    return 0;
}

int print_snake(int moves, int x[], int y[], int length)
{
    for (int i = 0;i<length;i++)
    {
        if (moves - i >= 0)
        {
            if (i == 0) // The head of snake
            {
                if (y[moves] - y[moves-1] < 0) // Up
                    color_str(y[moves-i], x[moves-i], COLOR_WHITE, COLOR_BLACK, "^");
                else if (y[moves] - y[moves-1] > 0) // Down
                    color_str(y[moves-i], x[moves-i], COLOR_WHITE, COLOR_BLACK, "v");

                if (x[moves] - x[moves-1] < 0) // Left
                    color_str(y[moves-i], x[moves-i], COLOR_WHITE, COLOR_BLACK, "<");
                else if (x[moves] - x[moves-1] > 0) // Right
                    color_str(y[moves-i], x[moves-i], COLOR_WHITE, COLOR_BLACK, ">");
            }
            else
                color_str(y[moves-i], x[moves-i], COLOR_WHITE, COLOR_BLACK, "#");
        }
        else
            color_str(y[MAX_SNAKE_LENGTH-i+moves], x[MAX_SNAKE_LENGTH-i+moves], COLOR_WHITE, COLOR_BLACK, "#");
    }

    refresh();
    return 0;
}

int print_food(int maxX, int maxY)
{
    struct timeval t;

    gettimeofday(&t, NULL);

    // Seed microseconds
    srand(t.tv_usec * t.tv_sec);

    // Get random range formula from http://c-faq.com/lib/randrange.html and http://stackoverflow.com/a/2509699
    int x_rand = ((maxX - 1) + 1) * ((double)rand()/RAND_MAX);
    int y_rand = ((((maxY - 1) - FOOTER_ROWS) - HEADER_ROWS + 1) * ((double)rand()/RAND_MAX)) + HEADER_ROWS;

    color_str(y_rand, x_rand, COLOR_WHITE, COLOR_BLACK, "§");

    refresh();

    return 0;
}

int main(int argc, char *argv[])
{
    WINDOW *mainwin;
    snake_param_t *snake_p;
    pthread_t thread_kb;

    snake_p = malloc(sizeof(snake_param_t));

    snake_p->length = 5;
    snake_p->speed = 1000000;

    mainwin = initscr();
    if (mainwin == NULL)
    {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(-1);
    }

    // Get the maximum size of the screen
    getmaxyx(mainwin, snake_p->maxY, snake_p->maxX);
    snake_p->x = snake_p->maxX / 2;
    snake_p->y = snake_p->maxY / 2;

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

    // The timeout and wtimeout routines set blocking or non-blocking 
    // read for a given window. If delay is negative, blocking read is used 
    // (i.e., waits indefinitely for input). If delay is zero, then non-blocking 
    // read is used (i.e., read returns ERR if no input is waiting). If delay is 
    // positive, then read blocks for delay milliseconds, and returns ERR 
    // if there is still no input.
    timeout(0);
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

    memset(snake_p->move_x, -1, sizeof snake_p->move_x);
    memset(snake_p->move_y, -1, sizeof snake_p->move_y);

    // Print header, footer, snake and food and then wait for a key
    print_header(snake_p->maxY, snake_p->maxX);
    print_snake(snake_p->moves, snake_p->move_x, snake_p->move_y, snake_p->length);
    print_food(snake_p->maxX, snake_p->maxY);
    print_footer(snake_p->maxY, snake_p->x, snake_p->y, snake_p->moves, snake_p->speed);


    pthread_create (&thread_kb, NULL, (void *) &get_keyboard_info, (void *) snake_p);

    pthread_join(thread_kb, NULL);

    while(snake_p->ch != 'q')
    {

        erase();

        print_header(snake_p->maxY, snake_p->maxX);
        print_snake(snake_p->moves, snake_p->move_x, snake_p->move_y, snake_p->length);
        print_food(snake_p->maxX, snake_p->maxY);
        print_footer(snake_p->maxY, snake_p->x, snake_p->y, snake_p->moves, snake_p->speed);
        usleep(snake_p->speed);
    }

    free(snake_p);
    delwin(mainwin);
    endwin();
    refresh();

    return 0;
}

