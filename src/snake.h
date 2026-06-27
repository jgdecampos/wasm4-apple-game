#pragma once

#include <stdbool.h>
#include <stdint.h>

#define SNAKE_CELL_SIZE 8
#define SNAKE_GRID_SIZE 20
#define SNAKE_MAX_LENGTH 28

typedef struct {
    int8_t x;
    int8_t y;
} Point;

typedef struct {
    Point body[SNAKE_MAX_LENGTH];
    Point direction;
    uint8_t length;
    uint8_t wanted_length;
    uint8_t move_delay;
    uint8_t move_timer;
} Snake;

void snake_init(Snake *snake, int8_t start_x, int8_t start_y, Point direction, uint8_t length, uint8_t move_delay);
void snake_update(Snake *snake, int16_t target_x, int16_t target_y);
void snake_grow(Snake *snake);
void snake_draw(const Snake *snake);
bool snake_hits_rect(const Snake *snake, int16_t x, int16_t y, int16_t width, int16_t height);
