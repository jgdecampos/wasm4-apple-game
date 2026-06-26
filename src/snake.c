#include "snake.h"
#include "wasm4.h"
#include <stdlib.h>

void snake_init(struct snake *snake) {
    if(snake->body != NULL)
    {
            free(snake->body);
            snake->body = NULL;
    }
    snake->length = 0;
}

void snake_push(struct snake *snake, struct point p) {
    struct point* body = realloc(snake->body, sizeof(struct point) * (snake->length+1));
    if(body)
    {
        snake->body = body;
        snake->body[snake->length++] = p;
    }
}

void snake_draw(struct snake *snake) {
    *DRAW_COLORS = 3;
    for(size_t i = 0; i < snake->length; ++i)
    {
        rect(snake->body[i].x*8, snake->body[i].y*8,8,8);
    }
}
