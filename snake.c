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

// Snake parameters
typedef struct snake_param
{
    int ch;
    int x;
    int y;
    int move_x[MAX_SNAKE_LENGTH];
    int move_y[MAX_SNAKE_LENGTH];
    short color_fg[MAX_SNAKE_LENGTH];
    int maxX;
    int maxY;
    int snake_maxX;
    int snake_maxY;
    int moves;
    int length;
    unsigned int speed;
} snake_param_t;

// Food parameters
typedef struct food_param
{
    int x;
    int y;
    int food_maxX;
    int food_maxY;
    int type;
    int isFirst;
} food_param_t;

snake_param_t snake_p;
food_param_t food_p;
pthread_mutex_t lock_snake;

int color_str(WINDOW *win, int y, int x, short fg_color, short bg_color, const char * str)
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

    mvwaddstr(win, y,x,str);

    attroff(COLOR_PAIR(i));
    return 0;
}

void *print_header(WINDOW *win)
{
    char buf[50];
    int header_width = 0;

    wclear(win);

    memset(buf, '\0', sizeof buf);
    int char_ret1 = snprintf(buf, sizeof buf, "Max Height: %d", snake_p.maxY);
    mvwaddstr(win, 0, 0, buf);
    header_width += char_ret1;

    memset(buf, '\0', sizeof buf);
    int char_ret2 = snprintf(buf, sizeof buf, "Max Width: %d", snake_p.maxX);
    mvwaddstr(win, 0, ++header_width, buf);
    header_width += char_ret2;

    wrefresh(win);
    usleep(10000);
}

void *print_footer(WINDOW *win)
{
    char buf[50];
    int char_ret[9], i = 0;
    int footer_width = 0;

    wclear(win);

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "FOOD:");
    mvwaddstr(win, 0, 0, buf);
    footer_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "x: %d", food_p.x);
    mvwaddstr(win, 0, ++footer_width, buf);
    footer_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "y: %d", food_p.y);
    mvwaddstr(win, 0, ++footer_width, buf);
    footer_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "SNAKE:");
    mvwaddstr(win, 0, ++footer_width, buf);
    footer_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "x: %d", snake_p.x);
    mvwaddstr(win, 0, ++footer_width, buf);
    footer_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "y: %d", snake_p.y);
    mvwaddstr(win, 0, ++footer_width, buf);
    footer_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "moves: %d", snake_p.moves);
    mvwaddstr(win, 0, ++footer_width, buf);
    footer_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "speed: %u", snake_p.speed);
    mvwaddstr(win, 0, ++footer_width, buf);
    footer_width += char_ret[i++];

    memset(buf, '\0', sizeof buf);
    char_ret[i] = snprintf(buf, sizeof buf, "length: %u", snake_p.length);
    mvwaddstr(win, 0, ++footer_width, buf);
    footer_width += char_ret[i++];

    wrefresh(win);
    usleep(10000);
}

void *print_food(WINDOW *win)
{
    struct timeval t;

    getmaxyx(win, food_p.food_maxY, food_p.food_maxX);

    gettimeofday(&t, NULL);

    // Seed microseconds
    srand(t.tv_usec * t.tv_sec);

    // If food is called for first time or the head of snake has the same coordinates
    // with the food, then calculate the new coordinates for food. Increase the length
    // of the snake.
    if (food_p.isFirst || (food_p.x == snake_p.x && food_p.y == snake_p.y))
    {
        // Get random range formula from http://c-faq.com/lib/randrange.html and http://stackoverflow.com/a/2509699
        food_p.x = food_p.food_maxX * ((double)rand()/RAND_MAX);
        food_p.y = food_p.food_maxY * ((double)rand()/RAND_MAX);

        food_p.isFirst = 0;

        snake_p.length += 1;
        if (snake_p.length >= MAX_SNAKE_LENGTH)
            snake_p.length = MAX_SNAKE_LENGTH;
    }

    color_str(win, food_p.y, food_p.x, COLOR_WHITE, COLOR_BLACK, "§");
}

void *print_snake(void *arg)
{
    WINDOW *win = (WINDOW *) arg;

    while(snake_p.ch != 'q')
    {
        wclear(win);

        print_food(win);

        for (int i = 0;i < snake_p.length;i++)
        {
            if (snake_p.moves - i >= 0)
            {
                if (i == 0) // The head of snake
                {
                    char * head;

                    if (snake_p.ch == KEY_UP)
                        head = "^";
                    else if (snake_p.ch == KEY_DOWN)
                        head = "v";
                    else if (snake_p.ch == KEY_LEFT)
                        head = "<";
                    else if (snake_p.ch == KEY_RIGHT)
                        head = ">";

                    color_str(win, snake_p.move_y[snake_p.moves - i], snake_p.move_x[snake_p.moves - i], snake_p.color_fg[snake_p.moves - i], COLOR_BLACK, head);
                }
                else
                {
                    if (snake_p.move_y[snake_p.moves - i] == snake_p.move_y[snake_p.moves] && snake_p.move_x[snake_p.moves - i] == snake_p.move_x[snake_p.moves])
                        snake_p.color_fg[snake_p.moves - i] = COLOR_RED;

                    color_str(win, snake_p.move_y[snake_p.moves - i], snake_p.move_x[snake_p.moves - i], snake_p.color_fg[snake_p.moves - i], COLOR_BLACK, "#");
                }
            }
            else
            {
                if (snake_p.move_y[MAX_SNAKE_LENGTH - i + snake_p.moves] == snake_p.move_y[MAX_SNAKE_LENGTH + snake_p.moves] && snake_p.move_x[MAX_SNAKE_LENGTH - i + snake_p.moves] == snake_p.move_x[MAX_SNAKE_LENGTH + snake_p.moves])
                    snake_p.color_fg[MAX_SNAKE_LENGTH - i + snake_p.moves] = COLOR_RED;

                color_str(win, snake_p.move_y[MAX_SNAKE_LENGTH - i + snake_p.moves], snake_p.move_x[MAX_SNAKE_LENGTH - i + snake_p.moves], snake_p.color_fg[MAX_SNAKE_LENGTH - i + snake_p.moves], COLOR_BLACK, "~");
            }
        }

        wrefresh(win);
        usleep(snake_p.speed);

        pthread_mutex_unlock(&lock_snake);
    }

    pthread_exit(0);
}

void *control_snake()
{
    int last_char = 0;

    while(snake_p.ch != 'q')
    {
        // Get keyboard input non-blocking
        snake_p.ch = getch();
        if (snake_p.ch == ERR)
            snake_p.ch = last_char;
        else if (snake_p.ch == 'q')
            break;

#if 1
        // If arrow keys pressed ones, don't have to press it again.
        if (snake_p.ch == KEY_UP || snake_p.ch == KEY_DOWN || snake_p.ch == KEY_LEFT || snake_p.ch == KEY_RIGHT)
            last_char = snake_p.ch;
#endif

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

                if ( snake_p.y >= snake_p.maxY - FOOTER_ROWS - 1)
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

        pthread_mutex_lock(&lock_snake);
    }

    pthread_exit(0);
}


int main(int argc, char *argv[])
{
    WINDOW *header_win, *footer_win, *food_win, *snake_win;
    pthread_t thread_snake, thread_control;

    memset(&snake_p, 0, sizeof snake_p);
    memset(snake_p.move_x, -1, sizeof snake_p.move_x);
    memset(snake_p.move_y, -1, sizeof snake_p.move_y);
    memset(snake_p.color_fg, COLOR_WHITE, sizeof snake_p.color_fg);
    memset(&food_p, 0, sizeof food_p);

    snake_p.length = 10;
    snake_p.speed = 1000000;

    food_p.isFirst = 1;

    initscr();

    // Check if colors are supported
    if (!has_colors())
    {
        delwin(header_win);
        delwin(footer_win);
        delwin(snake_win);
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

    // Get the maximum size of the screen
    getmaxyx(stdscr, snake_p.maxY, snake_p.maxX);

    // Create window for the header rows
    header_win = newwin(HEADER_ROWS, snake_p.maxX, 0, 0);

    // Create window for the footer rows
    footer_win = newwin(FOOTER_ROWS, snake_p.maxX, snake_p.maxY - FOOTER_ROWS, 0);


    // Create window for the snake game
    snake_win = newwin(snake_p.maxY - HEADER_ROWS - FOOTER_ROWS, snake_p.maxX, HEADER_ROWS, 0);


    getmaxyx(snake_win, snake_p.snake_maxY, snake_p.snake_maxX);

    // Start snake from the middle of screen.
    snake_p.x = snake_p.maxX / 2;
    snake_p.y = snake_p.maxY / 2;

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
    keypad(stdscr, TRUE);

    if (pthread_mutex_init(&lock_snake, NULL) != 0)
    {
        fprintf(stderr, "Mutex init failed\n");
        exit(-1);
    }

    // Add snake in a separate thread
    pthread_create(&thread_control, NULL, control_snake, NULL);
    pthread_create(&thread_snake, NULL, print_snake, (void *) snake_win);

    while(snake_p.ch != 'q')
    {
        print_header(header_win);
        print_footer(footer_win);
    }

    pthread_join(thread_control, NULL);
    pthread_join(thread_snake, NULL);
    pthread_mutex_destroy(&lock_snake);


    delwin(header_win);
    delwin(footer_win);
    delwin(snake_win);
    endwin();

    return 0;
}

