#ifndef SNAKE_H
#define SNAKE_H

#include <curses.h>

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
    int width;
    int height;
    int snake_width;
    int snake_height;
    int moves;
    int length;
    unsigned int speed;
    char set_border;
} snake_param_t;

void *print_snake(void *);
void *control_snake();

#endif /* SNAKE_H */


