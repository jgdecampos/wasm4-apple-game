#include "snake.h"
#include "wasm4.h"

static int8_t sign_cell(int16_t value) {
    if (value < 0) {
        return -1;
    }
    if (value > 0) {
        return 1;
    }
    return 0;
}

static bool is_opposite(Point a, Point b) {
    return a.x == -b.x && a.y == -b.y;
}

static bool direction_is_valid(const Snake *snake, Point direction) {
    Point head = snake->body[0];
    int8_t next_x = (int8_t)(head.x + direction.x);
    int8_t next_y = (int8_t)(head.y + direction.y);

    if (direction.x == 0 && direction.y == 0) {
        return false;
    }
    if (next_x < 0 || next_x >= SNAKE_GRID_SIZE || next_y < 0 || next_y >= SNAKE_GRID_SIZE) {
        return false;
    }
    return !is_opposite(direction, snake->direction);
}

static Point choose_direction(const Snake *snake, int16_t target_x, int16_t target_y) {
    Point head = snake->body[0];
    int16_t target_cell_x = target_x / SNAKE_CELL_SIZE;
    int16_t target_cell_y = target_y / SNAKE_CELL_SIZE;
    int16_t distance_x = target_cell_x - head.x;
    int16_t distance_y = target_cell_y - head.y;

    Point preferred[4];
    if (distance_x < 0) {
        distance_x = (int16_t)-distance_x;
    }
    if (distance_y < 0) {
        distance_y = (int16_t)-distance_y;
    }

    if (distance_x >= distance_y) {
        preferred[0] = (Point){sign_cell(target_cell_x - head.x), 0};
        preferred[1] = (Point){0, sign_cell(target_cell_y - head.y)};
    } else {
        preferred[0] = (Point){0, sign_cell(target_cell_y - head.y)};
        preferred[1] = (Point){sign_cell(target_cell_x - head.x), 0};
    }

    preferred[2] = snake->direction;
    preferred[3] = (Point){(int8_t)-snake->direction.y, snake->direction.x};

    for (uint8_t i = 0; i < 4; ++i) {
        if (direction_is_valid(snake, preferred[i])) {
            return preferred[i];
        }
    }

    return (Point){(int8_t)-snake->direction.x, (int8_t)-snake->direction.y};
}

void snake_init(Snake *snake, int8_t start_x, int8_t start_y, Point direction, uint8_t length, uint8_t move_delay) {
    if (length > SNAKE_MAX_LENGTH) {
        length = SNAKE_MAX_LENGTH;
    }

    snake->direction = direction;
    snake->length = length;
    snake->wanted_length = length;
    snake->move_delay = move_delay;
    snake->move_timer = 0;

    for (uint8_t i = 0; i < SNAKE_MAX_LENGTH; ++i) {
        snake->body[i].x = (int8_t)(start_x - (direction.x * (int8_t)i));
        snake->body[i].y = (int8_t)(start_y - (direction.y * (int8_t)i));
    }
}

void snake_update(Snake *snake, int16_t target_x, int16_t target_y) {
    snake->move_timer++;
    if (snake->move_timer < snake->move_delay) {
        return;
    }
    snake->move_timer = 0;

    snake->direction = choose_direction(snake, target_x, target_y);

    if (snake->length < snake->wanted_length) {
        snake->length++;
    }

    for (uint8_t i = (uint8_t)(snake->length - 1); i > 0; --i) {
        snake->body[i] = snake->body[i - 1];
    }

    snake->body[0].x = (int8_t)(snake->body[0].x + snake->direction.x);
    snake->body[0].y = (int8_t)(snake->body[0].y + snake->direction.y);
}

void snake_grow(Snake *snake) {
    if (snake->wanted_length < SNAKE_MAX_LENGTH) {
        snake->wanted_length++;
    }
}

void snake_draw(const Snake *snake) {
    *DRAW_COLORS = 3;
    for (uint8_t i = 0; i < snake->length; ++i) {
        rect(snake->body[i].x * SNAKE_CELL_SIZE, snake->body[i].y * SNAKE_CELL_SIZE, SNAKE_CELL_SIZE, SNAKE_CELL_SIZE);
    }

    *DRAW_COLORS = 4;
    rect(snake->body[0].x * SNAKE_CELL_SIZE + 2, snake->body[0].y * SNAKE_CELL_SIZE + 2, 4, 4);
}

bool snake_hits_rect(const Snake *snake, int16_t x, int16_t y, int16_t width, int16_t height) {
    for (uint8_t i = 0; i < snake->length; ++i) {
        int16_t snake_x = snake->body[i].x * SNAKE_CELL_SIZE;
        int16_t snake_y = snake->body[i].y * SNAKE_CELL_SIZE;
        bool separated = x + width <= snake_x ||
            x >= snake_x + SNAKE_CELL_SIZE ||
            y + height <= snake_y ||
            y >= snake_y + SNAKE_CELL_SIZE;

        if (!separated) {
            return true;
        }
    }

    return false;
}

