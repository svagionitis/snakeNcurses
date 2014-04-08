#ifndef FOOD_H
#define FOOD_H

#include <curses.h>

// Food parameters
typedef struct food_param
{
    int x;
    int y;
    int food_width;
    int food_height;
    int type;
    int isFirst;
} food_param_t;

void print_food(WINDOW *);

#endif /* FOOD_H */
