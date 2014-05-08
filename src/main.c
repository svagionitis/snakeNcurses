/*Build: gcc -D_BSD_SOURCE -std=c99 -o snake snake.c -lncurses -lpthread -Wall -W -O2*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <curses.h>
#include <pthread.h>
#include <signal.h>
#include "header.h"
#include "footer.h"
#include "utils.h"
#include "food.h"
#include "snake.h"


// Global variables
WINDOW *header_win, *footer_win, *snake_win;
snake_param_t snake_p;
food_param_t food_p;
pthread_mutex_t lock_snake;

void resize_all(int sig)
{
    fprintf(stderr, "sig: %d\n", sig);

    getmaxyx(stdscr, snake_p.height, snake_p.width);

    resizeterm(snake_p.height, snake_p.width);

#if 0
    wresize(header_win, HEADER_ROWS, snake_p.width);
    wresize(footer_win, FOOTER_ROWS, snake_p.width);
    wresize(snake_win, snake_p.height - HEADER_ROWS - FOOTER_ROWS, snake_p.width);
#endif
}

int main(int argc, char *argv[])
{
    pthread_t thread_snake, thread_control;

    if (argc != 1)
    {
        printf("Usage: %s\n", argv[0]);
        return -1;
    }

    memset(&snake_p, 0, sizeof snake_p);
    memset(snake_p.move_x, -1, sizeof snake_p.move_x);
    memset(snake_p.move_y, -1, sizeof snake_p.move_y);
    // Initialize color with the default fg
    memset(snake_p.color_fg, 0, sizeof snake_p.color_fg);
    memset(&food_p, 0, sizeof food_p);

    snake_p.length = 10;
    snake_p.speed = 1000000;

    food_p.isFirst = 1;

    // Catch resize signal
    signal(SIGWINCH, resize_all);

    initscr();

    // Check if colors are supported
    if (!has_colors())
    {
        endwin();
        fprintf(stderr,"Your terminal does not support color\n");
        return -1;
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
    getmaxyx(stdscr, snake_p.height, snake_p.width);

    // Create window for the header rows
    header_win = newwin(HEADER_ROWS, snake_p.width, 0, 0);

    // Create window for the footer rows
    footer_win = newwin(FOOTER_ROWS, snake_p.width, snake_p.height - FOOTER_ROWS, 0);


    // Create window for the snake game
    snake_win = newwin(snake_p.height - HEADER_ROWS - FOOTER_ROWS, snake_p.width, HEADER_ROWS, 0);

    // Get the maximum size of the snake window
    getmaxyx(snake_win, snake_p.snake_height, snake_p.snake_width);
    getmaxyx(snake_win, food_p.food_height, food_p.food_width);

    // Start snake from the middle of screen.
    snake_p.x = snake_p.width / 2;
    snake_p.y = snake_p.height / 2;

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
        return -1;
    }

    // Add snake in a separate thread
    pthread_create(&thread_control, NULL, control_snake, NULL);
    pthread_create(&thread_snake, NULL, print_snake, (void *) snake_win);

    while(snake_p.ch != 'q')
    {
        print_header(header_win);
        print_footer(footer_win);

        doupdate();

        usleep(10000);
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

