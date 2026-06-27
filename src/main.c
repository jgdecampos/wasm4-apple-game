#include "wasm4.h"
#include "snake.h"
#include <stdbool.h>
#include <stdint.h>

#define FPS 60
#define APPLE_SIZE 8
#define APPLE_SPEED 1
#define PHASE_DURATION_FRAMES ((uint16_t)(20 * FPS))
#define GROW_INTERVAL_FRAMES ((uint16_t)(4 * FPS))

typedef enum {
    ESTADO_INICIO,
    ESTADO_JOGANDO,
    ESTADO_FIM,
    ESTADO_PAUSE
} GameState;

static int16_t apple_x = 76;
static int16_t apple_y = 76;
static Snake snakes[3];
static uint8_t phase = 1;
static uint16_t phase_frames = 0;
static bool player_won = false;
static GameState state = ESTADO_INICIO;
static uint8_t previous_gamepad = 0;

static uint8_t active_snakes(void) {
    return phase;
}

static void reset_snakes(void) {
    snake_init(&snakes[0], 2, 1, (Point){1, 0}, 3, 12);
    snake_init(&snakes[1], 17, 1, (Point){-1, 0}, 3, 11);
    snake_init(&snakes[2], 2, 18, (Point){1, 0}, 3, 10);
}

static void start_phase(void) {
    apple_x = 76;
    apple_y = 76;
    phase_frames = 0;
    reset_snakes();
}

static void draw_background(void) {
    *DRAW_COLORS = 1;
    rect(0, 0, SCREEN_SIZE, SCREEN_SIZE);
}

static void draw_phase_label(void) {
    *DRAW_COLORS = 4;
    if (phase == 1) {
        text("FASE 1", 56, 8);
    } else if (phase == 2) {
        text("FASE 2", 56, 8);
    } else {
        text("FASE 3", 56, 8);
    }
}

static void draw_timer(void) {
    uint16_t seconds_left = (uint16_t)(20 - (phase_frames / FPS));
    char timer[] = "00s";

    if (seconds_left > 20) {
        seconds_left = 0;
    }

    timer[0] = (char)('0' + (seconds_left / 10));
    timer[1] = (char)('0' + (seconds_left % 10));

    *DRAW_COLORS = 4;
    text(timer, 128, 8);
}

static void draw_apple(void) {
    *DRAW_COLORS = 2;
    oval(apple_x, apple_y, APPLE_SIZE, APPLE_SIZE);
}

static void draw_start_screen(void) {
    draw_background();
    *DRAW_COLORS = 4;
    text("APPLE GAME", 40, 48);
    draw_phase_label();
    text("X PARA JOGAR", 32, 80);
    text("FUJA DA COBRA", 28, 104);
}

static void draw_pause_screen(void) {
    draw_background();
    draw_phase_label();
    *DRAW_COLORS = 4;
    text("PAUSE", 60, 64);
    text("Z CONTINUA", 36, 88);
}

static void draw_end_screen(void) {
    draw_background();
    *DRAW_COLORS = 4;
    if (player_won) {
        text("PARABENS!", 44, 56);
        text("VOCE VENCEU", 36, 72);
    } else {
        text("GAME OVER", 44, 64);
    }
    text("APERTE X", 48, 96);
}

static void move_apple(uint8_t gamepad) {
    if ((gamepad & BUTTON_LEFT) != 0 && apple_x > 0) {
        apple_x -= APPLE_SPEED;
    }
    if ((gamepad & BUTTON_RIGHT) != 0 && apple_x < SCREEN_SIZE - APPLE_SIZE) {
        apple_x += APPLE_SPEED;
    }
    if ((gamepad & BUTTON_UP) != 0 && apple_y > 0) {
        apple_y -= APPLE_SPEED;
    }
    if ((gamepad & BUTTON_DOWN) != 0 && apple_y < SCREEN_SIZE - APPLE_SIZE) {
        apple_y += APPLE_SPEED;
    }
}

static void update_phase(void) {
    phase_frames++;

    if ((phase_frames % GROW_INTERVAL_FRAMES) == 0 && phase_frames < PHASE_DURATION_FRAMES) {
        uint8_t count = active_snakes();
        for (uint8_t i = 0; i < count; ++i) {
            snake_grow(&snakes[i]);
        }
    }

    if (phase_frames >= PHASE_DURATION_FRAMES) {
        if (phase < 3) {
            phase++;
            start_phase();
            state = ESTADO_INICIO;
        } else {
            player_won = true;
            state = ESTADO_FIM;
        }
    }
}

static void update_playing(uint8_t gamepad, bool pressed_pause) {
    if (pressed_pause) {
        state = ESTADO_PAUSE;
        return;
    }

    move_apple(gamepad);

    uint8_t count = active_snakes();
    for (uint8_t i = 0; i < count; ++i) {
        snake_update(&snakes[i], apple_x, apple_y);
        if (snake_hits_rect(&snakes[i], apple_x, apple_y, APPLE_SIZE, APPLE_SIZE)) {
            player_won = false;
            state = ESTADO_FIM;
        }
    }

    if (state == ESTADO_JOGANDO) {
        update_phase();
    }
}

static void draw_playing(void) {
    draw_background();
    draw_phase_label();
    draw_timer();

    uint8_t count = active_snakes();
    for (uint8_t i = 0; i < count; ++i) {
        snake_draw(&snakes[i]);
    }
    draw_apple();
}

void start(void) {
    PALETTE[0] = 0xfcdeea;
    PALETTE[1] = 0xff4d6d;
    PALETTE[2] = 0x265935;
    PALETTE[3] = 0x012824;

    start_phase();
}

void update(void) {
    uint8_t gamepad = *GAMEPAD1;
    bool pressed_x = (gamepad & BUTTON_1) != 0 && (previous_gamepad & BUTTON_1) == 0;
    bool pressed_pause = (gamepad & BUTTON_2) != 0 && (previous_gamepad & BUTTON_2) == 0;

    if (state == ESTADO_INICIO) {
        draw_start_screen();
        if (pressed_x) {
            state = ESTADO_JOGANDO;
        }
    } else if (state == ESTADO_JOGANDO) {
        update_playing(gamepad, pressed_pause);
        draw_playing();
    } else if (state == ESTADO_PAUSE) {
        draw_pause_screen();
        if (pressed_pause) {
            state = ESTADO_JOGANDO;
        }
    } else {
        draw_end_screen();
        if (pressed_x) {
            phase = 1;
            player_won = false;
            start_phase();
            state = ESTADO_INICIO;
        }
    }

    previous_gamepad = gamepad;
}
