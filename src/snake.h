#ifndef SNAKE_H
#define SNAKE_H

#include <curses.h>
#include <stdint.h>

#define MAX_SNAKE_LENGTH 50

// Snake parameters
typedef struct snake_param
{
    int32_t     ch;
    int32_t     last_char;
    int32_t     x;
    int32_t     y;
    int32_t     move_x[MAX_SNAKE_LENGTH];
    int32_t     move_y[MAX_SNAKE_LENGTH];
    int16_t     color_fg[MAX_SNAKE_LENGTH];
    int32_t     width;
    int32_t     height;
    int32_t     snake_width;
    int32_t     snake_height;
    int32_t     moves;
    int32_t     length;
    uint32_t    speed;
    int8_t      set_border;
} snake_param_t;

void *print_snake(void *);

#endif /* SNAKE_H */


