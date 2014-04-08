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

        print_food(win);

        // Get the max snake window size because the window could be 
        // resized and we want to update it.
        getmaxyx(win, snake_p.snake_height, snake_p.snake_width);

        char *body = NULL;

        for (int i = 0;i < snake_p.length;i++)
        {
            if (snake_p.moves - i >= 0)
            {
                if (i == 0) // The head of snake
                {
                    char *head = NULL;

                    if (snake_p.ch == KEY_UP)
                    {
                        head = "^";
                        body = "@";
                    }
                    else if (snake_p.ch == KEY_DOWN)
                    {
                        head = "v";
                        body = "@";
                    }
                    else if (snake_p.ch == KEY_LEFT)
                    {
                        head = "<";
                        body = "@";
                    }
                    else if (snake_p.ch == KEY_RIGHT)
                    {
                        head = ">";
                        body = "@";
                    }

                    color_str(win, snake_p.move_y[snake_p.moves - i], snake_p.move_x[snake_p.moves - i], snake_p.color_fg[snake_p.moves - i], COLOR_BLACK, head);
                }
                }
                else
                {
#if 1
                    if (snake_p.move_y[snake_p.moves - i] == snake_p.move_y[snake_p.moves] && snake_p.move_x[snake_p.moves - i] == snake_p.move_x[snake_p.moves])
                        snake_p.color_fg[snake_p.moves - i] = COLOR_RED;
#endif

                    color_str(win, snake_p.move_y[snake_p.moves - i], snake_p.move_x[snake_p.moves - i], snake_p.color_fg[snake_p.moves - i], COLOR_BLACK, body);

                    snake_p.color_fg[snake_p.moves - i] = 0;

                }
            }
            else
            {
                color_str(win, snake_p.move_y[MAX_SNAKE_LENGTH - i + snake_p.moves], snake_p.move_x[MAX_SNAKE_LENGTH - i + snake_p.moves], snake_p.color_fg[MAX_SNAKE_LENGTH - i + snake_p.moves], COLOR_BLACK, body);
            }
        }

        if (snake_p.set_border)
            wborder(win, '|', '|', '-', '_', '+', '+', '+', '+');

        wnoutrefresh(win);
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

                if (snake_p.set_border)
                {
                    if (snake_p.y <= 0)
                        snake_p.y = snake_p.snake_height - 1;
                }
                else
                {
                    if (snake_p.y <= -1)
                        snake_p.y = snake_p.snake_height;
                }

                snake_p.moves++;
                if (snake_p.moves >= MAX_SNAKE_LENGTH)
                    snake_p.moves = 0;

                snake_p.move_x[snake_p.moves] = snake_p.x;
                snake_p.move_y[snake_p.moves] = snake_p.y;

                break;
            case KEY_DOWN:
                snake_p.y += 1;

                if (snake_p.set_border)
                {
                    if (snake_p.y >= snake_p.snake_height - 1)
                        snake_p.y = 1;
                }
                else
                {
                    if (snake_p.y >= snake_p.snake_height)
                        snake_p.y = 0;
                }

                snake_p.moves++;
                if (snake_p.moves >= MAX_SNAKE_LENGTH)
                    snake_p.moves = 0;

                snake_p.move_x[snake_p.moves] = snake_p.x;
                snake_p.move_y[snake_p.moves] = snake_p.y;

                break;
            case KEY_RIGHT:
                snake_p.x += 1;

                if (snake_p.set_border)
                {
                    if (snake_p.x >= snake_p.snake_width - 2)
                        snake_p.x = 1;
                }
                else
                {
                    if (snake_p.x >= snake_p.snake_width - 1)
                        snake_p.x = 0;
                }

                snake_p.moves++;
                if (snake_p.moves >= MAX_SNAKE_LENGTH)
                    snake_p.moves = 0;

                snake_p.move_x[snake_p.moves] = snake_p.x;
                snake_p.move_y[snake_p.moves] = snake_p.y;

                break;
            case KEY_LEFT:
                snake_p.x -= 1;

                if (snake_p.set_border)
                {
                    if (snake_p.x <= 1)
                        snake_p.x = snake_p.snake_width - 2;
                }
                else
                {
                    if (snake_p.x <= 0)
                        snake_p.x = snake_p.snake_width - 1;
                }

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
            case 'b':
                if (snake_p.set_border)
                {
                    snake_p.set_border = FALSE;
                }
                else
                {
                    snake_p.set_border = TRUE;
                }

                break;
        }

        pthread_mutex_lock(&lock_snake);
    }

    pthread_exit(0);
}

