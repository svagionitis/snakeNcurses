#include <stdint.h>
#include <pthread.h>
#include "control.h"
#include "snake.h"

extern snake_param_t snake_p;
extern pthread_mutex_t lock_snake;

void *control_snake()
{

    while(snake_p.ch != 'q')
    {
        // Get keyboard input non-blocking
        snake_p.ch = getch();
        if (snake_p.ch == ERR)
            snake_p.ch = snake_p.last_char;
        else if (snake_p.ch == 'q')
            break;

#if 1
        // If arrow keys pressed ones, don't have to press it again.
        if (snake_p.ch == KEY_UP || snake_p.ch == KEY_DOWN || snake_p.ch == KEY_LEFT || snake_p.ch == KEY_RIGHT)
            snake_p.last_char = snake_p.ch;
#endif

        // In thread with the sleep
        switch(snake_p.ch)
        {
            case KEY_UP:
                snake_p.y -= 1;

                if (snake_p.set_border)
                {
                    if (snake_p.y <= 0)
                        snake_p.y = snake_p.snake_height - 2;
                }
                else
                {
                    if (snake_p.y <= -1)
                        snake_p.y = snake_p.snake_height - 1;
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

