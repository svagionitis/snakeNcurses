#include <unistd.h>
#include <pthread.h>
#include "utils.h"
#include "food.h"
#include "snake.h"

extern snake_param_t snake_p;
extern pthread_mutex_t lock_snake;

void *print_snake(void *arg)
{
    WINDOW *win = (WINDOW *) arg;

    while(snake_p.ch != 'q')
    {
        wclear(win);

        // Get the max snake window size because the window could be 
        // resized and we want to update it.
        getmaxyx(win, snake_p.snake_height, snake_p.snake_width);

        for (int32_t i = 0;i < snake_p.length;i++)
        {
            int32_t body = '@';

            if (snake_p.moves - i >= 0)
            {
                if (i == 0) // The head of snake
                {
                    int32_t head = '\0';

                    if (snake_p.ch == KEY_UP)
                    {
                        head = '^';
                    }
                    else if (snake_p.ch == KEY_DOWN)
                    {
                        head = 'v';
                    }
                    else if (snake_p.ch == KEY_LEFT)
                    {
                        head = '<';
                    }
                    else if (snake_p.ch == KEY_RIGHT)
                    {
                        head = '>';
                    }

                    color_str(win, snake_p.move_y[snake_p.moves - i], snake_p.move_x[snake_p.moves - i], snake_p.color_fg[snake_p.moves - i], COLOR_BLACK, (const char *)&head);
                }
                else
                {
#if 1
                    if (snake_p.move_y[snake_p.moves - i] == snake_p.move_y[snake_p.moves] && snake_p.move_x[snake_p.moves - i] == snake_p.move_x[snake_p.moves])
                        snake_p.color_fg[snake_p.moves - i] = COLOR_RED;
#endif

                    color_str(win, snake_p.move_y[snake_p.moves - i], snake_p.move_x[snake_p.moves - i], snake_p.color_fg[snake_p.moves - i], COLOR_BLACK, (const char *)&body);

                    snake_p.color_fg[snake_p.moves - i] = 0;

                }
            }
            else
            {
                color_str(win, snake_p.move_y[MAX_SNAKE_LENGTH - i + snake_p.moves], snake_p.move_x[MAX_SNAKE_LENGTH - i + snake_p.moves], snake_p.color_fg[MAX_SNAKE_LENGTH - i + snake_p.moves], COLOR_BLACK, (const char *)&body);
            }
        }

        print_food(win);

        if (snake_p.set_border)
            wborder(win, '|', '|', '-', '_', '+', '+', '+', '+');

        wnoutrefresh(win);
        usleep(snake_p.speed);

        pthread_mutex_unlock(&lock_snake);
    }

    pthread_exit(0);
}

