#include "wasm4.h"
#include "snake.h"
#include <stdlib.h>
#include <cstdint>

// Estados do jogo
#define ESTADO_INICIO 0
#define ESTADO_JOGANDO 1
#define ESTADO_FIM 2
#define ESTADO_PAUSADO 3
#define ESTADO_VITORIA 4

int maca_x = 76;
int maca_y = 76;

Cobra cobras[3];
int fase = 1;
int crescimento = 0;
const int TAMANHO_MAX = 15;

int estado = ESTADO_INICIO;
uint8_t gamepad_anterior = 0;

void iniciar_fase() {
    maca_x = 76;
    maca_y = 76;

    for (int i = 0; i < 3; i++) {
        cobras[i].tamanho = 3;
        cobras[i].velocidade = 1;

        cobras[i].corpo[0].x = 2 + (i * 5);
        cobras[i].corpo[0].y = 2;
        cobras[i].corpo[1].x = cobras[i].corpo[0].x; // Resto do corpo fica escondido
        cobras[i].corpo[1].y = cobras[i].corpo[0].y;
        cobras[i].corpo[2].x = cobras[i].corpo[0].x;
        cobras[i].corpo[2].y = cobras[i].corpo[0].y;
    }
}

void start () {
    PALETTE[0] = 0xfcdeea; // Bege
    PALETTE[1] = 0xff4d6d; // Preto
    PALETTE[2] = 0x265935; // Vermelho
    PALETTE[3] = 0x012824; // Verde

    iniciar_fase();
}

void update () {
    uint32_t gamepad = *GAMEPAD1;

    int press_x = (gamepad & BUTTON_1) && !(gamepad_anterior & BUTTON_1);

    if (estado_atual == ESTADO_INICIO) {
        *DRAW_COLORS = 4;
        if (fase == 1) text("Fase 1", 55, 60);
        if (fase == 2) text("Fase 2", 55, 60);
        if (fase == 3) text("Fase 3", 55, 60);
        text("Pressione X", 35, 80);
        if (press_x) {
            estado = ESTADO_JOGANDO;
        }
    } else if (estado_atual == ESTADO_FIM) {
        *DRAW_COLORS = 2;
        text("GAME OVER!", 40, 60);
        *DRAW_COLORS = 4;
        text("Pressione X", 35, 80);
        if (press_x) {
            fase = 1;
            iniciar_fase()
            estado_atual = ESTADO_INICIO;
        }
    } else if (estado_atual == ESTADO_VITORIA) {
        *DRAW_COLORS = 3;
        text("Parabéns! Você venceu!", 35, 60);
        *DRAW_COLORS = 4;
        text("Pressione X", 35, 80);

        if (press_x) {
            fase = 1;
            iniciar_fase()
            estado_atual = ESTADO_INICIO;
        }
    } else if (estado_atual == ESTADO_JOGANDO) {
        velocidade++;

        if (gamepad && BUTTON_LEFT && maca_x > 0) {maca_x -= 1;}
        if (gamepad && BUTTON_RIGHT && maca_x < 127) {maca_x += 1;}
        if (gamepad && BUTTON_UP && maca_y > 0) {maca_y -= 1;}
        if (gamepad && BUTTON_DOWN && maca_y < 127) {maca_y += 1;}

    text("Apple game", 10, 10);

    uint8_t gamepad = *GAMEPAD1;
    if (gamepad & BUTTON_LEFT) {
        maca_x -= 1;
    }
    if (gamepad & BUTTON_RIGHT) {
        maca_x += 1;
    }
    if (gamepad & BUTTON_UP) {
        maca_y -= 1;
    }
    if (gamepad & BUTTON_DOWN) {
        maca_y += 1;
    }


    rect(maca_x, maca_y, 8, 8);

}
