#pragma once

#include <stdint.h>

struct point {
    int16_t x;
    int16_t y;
};

struct snake {
    struct point* body;
    struct point direction;
    uint16_t length;
};

void snake_init(struct snake *snake);

void snake_push(struct snake *snake, struct point p);

void snake_draw(struct snake *snake);
