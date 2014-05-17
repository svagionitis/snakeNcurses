#ifndef FOOD_H
#define FOOD_H

#include <curses.h>
#include <stdint.h>

/**
 * Food parameters
 */
typedef struct food_param
{
    int32_t x;              /**< x coordinate for food */
    int32_t y;              /**< y coordinate for food */
    int32_t food_width;     /**< The width of the game window */
    int32_t food_height;    /**< The height of the game window */
    int32_t type;           /**< What type of food. TODO */
    int8_t isFirst;         /**< True if the food appears for the first time. */
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
