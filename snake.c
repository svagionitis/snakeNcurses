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

snake_param_t snake_p;

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

void print_header()
{
    char buf[50];
    int header_width = 0;

    memset(buf, '\0', sizeof buf);
    int char_ret1 = snprintf(buf, sizeof buf, "Max Height: %d", snake_p.maxY);
    mvaddstr(0, 0, buf);
    header_width += char_ret1;

    memset(buf, '\0', sizeof buf);
    int char_ret2 = snprintf(buf, sizeof buf, "Max Width: %d", snake_p.maxX);
    mvaddstr(0, ++header_width, buf);
    header_width += char_ret2;
}

void print_footer()
{
    char buf[50];
    int footer_width = 0;

    memset(buf, '\0', sizeof buf);
    int char_ret1 = snprintf(buf, sizeof buf, "x: %d", snake_p.x);
    mvaddstr(snake_p.maxY - 1, 0, buf);
    footer_width += char_ret1;

    memset(buf, '\0', sizeof buf);
    int char_ret2 = snprintf(buf, sizeof buf, "y: %d", snake_p.y);
    mvaddstr(snake_p.maxY - 1, ++footer_width, buf);
    footer_width += char_ret2;

    memset(buf, '\0', sizeof buf);
    int char_ret3 = snprintf(buf, sizeof buf, "moves: %d", snake_p.moves);
    mvaddstr(snake_p.maxY - 1, ++footer_width, buf);
    footer_width += char_ret3;

    memset(buf, '\0', sizeof buf);
    int char_ret4 = snprintf(buf, sizeof buf, "speed: %u", snake_p.speed);
    mvaddstr(snake_p.maxY - 1, ++footer_width, buf);
    footer_width += char_ret4;
}

void print_snake()
{
    for (int i = 0;i<snake_p.length;i++)
    {
        if (snake_p.moves - i >= 0)
        {
            if (i == 0) // The head of snake
            {
                if (snake_p.move_y[snake_p.moves] - snake_p.move_y[snake_p.moves-1] < 0) // Up
                    color_str(snake_p.move_y[snake_p.moves-i], snake_p.move_x[snake_p.moves-i], COLOR_WHITE, COLOR_BLACK, "^");
                else if (snake_p.move_y[snake_p.moves] - snake_p.move_y[snake_p.moves-1] > 0) // Down
                    color_str(snake_p.move_y[snake_p.moves-i], snake_p.move_x[snake_p.moves-i], COLOR_WHITE, COLOR_BLACK, "v");

                if (snake_p.move_x[snake_p.moves] - snake_p.move_x[snake_p.moves-1] < 0) // Left
                    color_str(snake_p.move_y[snake_p.moves-i], snake_p.move_x[snake_p.moves-i], COLOR_WHITE, COLOR_BLACK, "<");
                else if (snake_p.move_x[snake_p.moves] - snake_p.move_x[snake_p.moves-1] > 0) // Right
                    color_str(snake_p.move_y[snake_p.moves-i], snake_p.move_x[snake_p.moves-i], COLOR_WHITE, COLOR_BLACK, ">");
            }
            else
                color_str(snake_p.move_y[snake_p.moves-i], snake_p.move_x[snake_p.moves-i], COLOR_WHITE, COLOR_BLACK, "#");
        }
        else
            color_str(snake_p.move_y[MAX_SNAKE_LENGTH-i+snake_p.moves], snake_p.move_x[MAX_SNAKE_LENGTH-i+snake_p.moves], COLOR_WHITE, COLOR_BLACK, "#");
    }
}

void print_food()
{
    struct timeval t;

    gettimeofday(&t, NULL);

    // Seed microseconds
    srand(t.tv_usec * t.tv_sec);

    // Get random range formula from http://c-faq.com/lib/randrange.html and http://stackoverflow.com/a/2509699
    int x_rand = ((snake_p.maxX - 1) + 1) * ((double)rand()/RAND_MAX);
    int y_rand = ((((snake_p.maxY - 1) - FOOTER_ROWS) - HEADER_ROWS + 1) * ((double)rand()/RAND_MAX)) + HEADER_ROWS;

    color_str(y_rand, x_rand, COLOR_WHITE, COLOR_BLACK, "§");
}

void control_snake()
{
    while(snake_p.ch != 'q')
    {
        clear();

        int last_char;

        // Get keyboard input non-blocking
        snake_p.ch = getch();
        if (snake_p.ch == ERR)
            snake_p.ch = last_char;
        else if (snake_p.ch == 'q')
            break;

        if (snake_p.ch == KEY_UP || snake_p.ch == KEY_DOWN || snake_p.ch == KEY_LEFT || snake_p.ch == KEY_RIGHT)
            last_char = snake_p.ch;

        // In thread with the sleep
        switch(snake_p.ch)
        {
            case KEY_UP:
                snake_p.y -= 1;

                if (snake_p.y < 0 + HEADER_ROWS)
                    snake_p.y = snake_p.maxY - FOOTER_ROWS - 1;

                snake_p.moves++;
                if (snake_p.moves >= MAX_SNAKE_LENGTH)
                    snake_p.moves = 0;

                snake_p.move_x[snake_p.moves] = snake_p.x;
                snake_p.move_y[snake_p.moves] = snake_p.y;

                break;
            case KEY_DOWN:
                snake_p.y += 1;

                if ( snake_p.y >= snake_p.maxY - FOOTER_ROWS)
                    snake_p.y = 0 + HEADER_ROWS;

                snake_p.moves++;
                if (snake_p.moves >= MAX_SNAKE_LENGTH)
                    snake_p.moves = 0;

                snake_p.move_x[snake_p.moves] = snake_p.x;
                snake_p.move_y[snake_p.moves] = snake_p.y;

                break;
            case KEY_RIGHT:
                snake_p.x += 1;

                if (snake_p.x > snake_p.maxX)
                    snake_p.x = 0;

                snake_p.moves++;
                if (snake_p.moves >= MAX_SNAKE_LENGTH)
                    snake_p.moves = 0;

                snake_p.move_x[snake_p.moves] = snake_p.x;
                snake_p.move_y[snake_p.moves] = snake_p.y;

                break;
            case KEY_LEFT:
                snake_p.x -= 1;

                if (snake_p.x < 0)
                    snake_p.x = snake_p.maxX;

                snake_p.moves++;
                if (snake_p.moves >= MAX_SNAKE_LENGTH)
                    snake_p.moves = 0;

                snake_p.move_x[snake_p.moves] = snake_p.x;
                snake_p.move_y[snake_p.moves] = snake_p.y;

                break;
            case '+':
                snake_p.speed += 100;

                break;
            case '-':
                snake_p.speed -= 100;

                break;
            case '*':
                snake_p.speed *= 10;

                break;
            case '/':
                snake_p.speed /= 10;

                if (snake_p.speed < 1)
                    snake_p.speed = 1;

                break;
        }

        print_snake();

        refresh();
        usleep(snake_p.speed);
    }
}


int main(int argc, char *argv[])
{
    WINDOW *mainwin;

    if (argc != 1)
    {
        printf("Usage: %s\n", argv[0]);
        return -1;
    }

    memset(&snake_p, 0, sizeof snake_p);

    snake_p.length = 5;
    snake_p.speed = 1000000;

    mainwin = initscr();
    if (mainwin == NULL)
    {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(-1);
    }

    // Get the maximum size of the screen
    getmaxyx(mainwin, snake_p.maxY, snake_p.maxX);
    snake_p.x = snake_p.maxX / 2;
    snake_p.y = snake_p.maxY / 2;

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

    memset(snake_p.move_x, -1, sizeof snake_p.move_x);
    memset(snake_p.move_y, -1, sizeof snake_p.move_y);

    //pthread_create(&thread_snake, NULL, threadfunc, NULL);


    while(snake_p.ch != 'q')
    {
        clear();

        print_header();
        print_food();
        print_footer();

        control_snake();

        refresh();
    }

    //pthread_join(thread_snake, NULL);

    delwin(mainwin);
    endwin();
    refresh();

    return 0;
}

