#ifndef FOOD_H
#define FOOD_H

#include <curses.h>

/**
 * Food parameters
 */
typedef struct food_param
{
    int x;              /**< x coordinate for food */
    int y;              /**< y coordinate for food */
    int food_width;     /**< The width of the game window */
    int food_height;    /**< The height of the game window */
    int type;           /**< What type of food. TODO */
    int isFirst;        /**< True if the food appears for the first time. */
} food_param_t;

/**
 * \brief Print the food on the screen using the given window
 * \param w A window where the food will be printed
 *
 * Using the input window print the food at the specified
 * coordinates.
 */
void print_food(WINDOW *w);

#endif /* FOOD_H */
