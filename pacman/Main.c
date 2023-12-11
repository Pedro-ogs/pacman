
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>

// ---------- GENERAL -----------

long frames;
long score;

void must_init(bool test, const char* description)
{
    if (test)
        return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

int between(int lo, int hi)
{
    return lo + (rand() % (hi - lo));
}

float between_f(float lo, float hi)
{
    return lo + ((float)rand() / (float)RAND_MAX) * (hi - lo);
}

bool collide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
{
    if (ax1 > bx2)
        return false;
    if (ax2 < bx1)
        return false;
    if (ay1 > by2)
        return false;
    if (ay2 < by1)
        return false;

    return true;
}

// ---------- DISPLAY ----------

//#define BUFFER_W 403
//#define BUFFER_H 364
#define BUFFER_W 364
#define BUFFER_H 403

#define DISP_SCALE 2
#define DISP_W (BUFFER_W * DISP_SCALE)
#define DISP_H (BUFFER_H * DISP_SCALE)

ALLEGRO_DISPLAY* disp;
ALLEGRO_BITMAP* buffer;

ALLEGRO_BITMAP* fundo;

void disp_init()
{
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

    disp = al_create_display(DISP_W, DISP_H);
    must_init(disp, "display");

    buffer = al_create_bitmap(BUFFER_W, BUFFER_H);
    must_init(buffer, "bitmap buffer");
}

void disp_deinit()
{
    al_destroy_bitmap(buffer);
    al_destroy_display(disp);
}

void disp_pre_draw()
{
    al_set_target_bitmap(buffer);
}

void disp_post_draw()
{
    al_set_target_backbuffer(disp);
    al_draw_scaled_bitmap(buffer, 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISP_W, DISP_H, 0);

    al_flip_display();
}

// ---------- KEYBOARD ----------

#define KEY_SEEN 1
#define KEY_RELEASED 2
unsigned char key[ALLEGRO_KEY_MAX];

void keyboard_init()
{
    memset(key, 0, sizeof(key));
}

void keyboard_update(ALLEGRO_EVENT* event)
{
    switch (event->type)
    {
    case ALLEGRO_EVENT_TIMER:
        for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
            key[i] &= KEY_SEEN;
        break;

    case ALLEGRO_EVENT_KEY_DOWN:
        key[event->keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
        break;
    case ALLEGRO_EVENT_KEY_UP:
        key[event->keyboard.keycode] &= KEY_RELEASED;
        break;
    }
}

// ---------- SPRITES ----------

#define PACMAN_W 13 
#define PACMAN_H 13 
#define FANTASMA_W 13
#define FANTASMA_H 13

typedef struct SPRITES
{
    ALLEGRO_BITMAP* _sheet;

    ALLEGRO_BITMAP* pacman[4]; // direita, esquerda, cima, baixo

    ALLEGRO_BITMAP* fantasmaVermelho[4]; //direita, esquerda, cima, baixo

    ALLEGRO_BITMAP* fantasmaRosa[4]; //direita, esquerda, cima, baixo

    ALLEGRO_BITMAP* fantasmaAzul[4]; //direita, esquerda, cima, baixo

    ALLEGRO_BITMAP* fantasmaLaranja[4]; //direita, esquerda, cima, baixo


} SPRITES;
SPRITES sprites;

ALLEGRO_BITMAP* sprite_grab(int x, int y, int w, int h)
{
    ALLEGRO_BITMAP* sprite = al_create_sub_bitmap(sprites._sheet, x, y, w, h);
    must_init(sprite, "sprite grab");
    return sprite;
}

void sprites_init()
{
    sprites._sheet = al_load_bitmap("spritesheet.png");
    must_init(sprites._sheet, "spritesheet");

    // pixel dentro da spritesheet (x,y,w,h)
    sprites.pacman[0] = sprite_grab(20, 1, PACMAN_W, PACMAN_H);  // direita
    sprites.pacman[1] = sprite_grab(21, 17, PACMAN_W, PACMAN_H); // esquerda
    sprites.pacman[2] = sprite_grab(20, 34, PACMAN_W, PACMAN_H); // cima
    sprites.pacman[3] = sprite_grab(20, 49, PACMAN_W, PACMAN_H); // baixo

    sprites.fantasmaVermelho[0] = sprite_grab(20, 65, FANTASMA_W, FANTASMA_H);  // direita
    sprites.fantasmaVermelho[1] = sprite_grab(36, 65, FANTASMA_W, FANTASMA_H);  // esquerda
    sprites.fantasmaVermelho[2] = sprite_grab(68, 65, FANTASMA_W, FANTASMA_H);  // cima
    sprites.fantasmaVermelho[3] = sprite_grab(100, 65, FANTASMA_W, FANTASMA_H); // baixo

    sprites.fantasmaRosa[0] = sprite_grab(20, 81, FANTASMA_W, FANTASMA_H);  // direita
    sprites.fantasmaRosa[1] = sprite_grab(36, 81, FANTASMA_W, FANTASMA_H);  // esquerda
    sprites.fantasmaRosa[2] = sprite_grab(68, 81, FANTASMA_W, FANTASMA_H);  // cima
    sprites.fantasmaRosa[3] = sprite_grab(100, 81, FANTASMA_W, FANTASMA_H); // baixo

    sprites.fantasmaAzul[0] = sprite_grab(20, 97, FANTASMA_W, FANTASMA_H);  // direita
    sprites.fantasmaAzul[1] = sprite_grab(36, 97, FANTASMA_W, FANTASMA_H);  // esquerda
    sprites.fantasmaAzul[2] = sprite_grab(68, 97, FANTASMA_W, FANTASMA_H);  // cima
    sprites.fantasmaAzul[3] = sprite_grab(100, 97, FANTASMA_W, FANTASMA_H); // baixo

    sprites.fantasmaLaranja[0] = sprite_grab(20, 113, FANTASMA_W, FANTASMA_H);  // direita
    sprites.fantasmaLaranja[1] = sprite_grab(36, 113, FANTASMA_W, FANTASMA_H);  // esquerda
    sprites.fantasmaLaranja[2] = sprite_grab(68, 113, FANTASMA_W, FANTASMA_H);  // cima
    sprites.fantasmaLaranja[3] = sprite_grab(100, 113, FANTASMA_W, FANTASMA_H); // baixo

}

void sprites_deinit()
{
    for (int i = 0; i < 3; i++)
    {
        al_destroy_bitmap(sprites.pacman[i]);
        al_destroy_bitmap(sprites.fantasmaVermelho[i]);
        al_destroy_bitmap(sprites.fantasmaRosa[i]);
        al_destroy_bitmap(sprites.fantasmaAzul[i]);
        al_destroy_bitmap(sprites.fantasmaLaranja[i]);
    }
    al_destroy_bitmap(sprites._sheet);
}

// ---------- AUDIO ----------

// ------------ MAPA --------------

#define TAM_X 28
#define TAM_Y 31

// 0 = vazio
// 1 = parede
// 2 = comida
// 3 = vitamina

int mapa[TAM_Y][TAM_X] = {
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, },
            {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, },
            {1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, },
            {1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, },
            {1, 3, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 3, 1, },
            {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, },
            {1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, },
            {1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, },
            {1, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 1, },
            {1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, },
            {1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, },
            {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1, },
            {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1, },
            {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1, },
            {4, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 4, },
            {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1, },
            {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1, },
            {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1, },
            {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1, },
            {1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1, },
            {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, },
            {1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, },
            {1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, },
            {1, 3, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 3, 1, },
            {1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, },
            {1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, },
            {1, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 1, },
            {1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, },
            {1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, },
            {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, },
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, }
};


void mapa_init()
{
}
void mapa_update()
{
}

void mapa_draw()
{
    int i, j;
    float x, y, h = (float)BUFFER_H / (TAM_Y), w = (float)BUFFER_W / (TAM_X);
    for (i = 0; i < TAM_Y; i++)
    {
        for (j = 0; j < TAM_X; j++)
        {
            y = i * h;
            x = j * w;

            if (i == 12 && j == 13)
                al_draw_filled_rectangle(x, y, x + w, y + h, al_map_rgba_f(1, 0, 0.5, 1));
            else if (i == 12 && j == 14)
                al_draw_filled_rectangle(x, y, x + w, y + h, al_map_rgba_f(1, 0, 0.5, 1));
            else if (mapa[i][j] == 1) // paredes
                al_draw_filled_rectangle(x, y, x + w, y + h, al_map_rgba_f(0, 0, 0.5, 1));
            else if (mapa[i][j] == 2) // comida
                al_draw_filled_rectangle(x - 1.5 + w / 2, y - 1.5 + h / 2, x + 1.5 + w / 2, y + 1.5 + h / 2, al_map_rgba_f(1, 1, 1, 1));
            else if (mapa[i][j] == 3) // vitamina
                al_draw_filled_rectangle(x - 1.5 + w / 2, y - 1.5 + h / 2, x + 1.5 + w / 2, y + 1.5 + h / 2, al_map_rgba_f(1, 0, 0, 1));

        }
    }

}
// ---------- FANTASMAS ----------

typedef struct FANTASMA
{
    int x, y;
    int dx, dy;
    int timer;
    int speed;
    int posit;
    int direcao;
} FANTASMA;

FANTASMA fantasmaVermelho;
FANTASMA fantasmaRosa;
FANTASMA fantasmaAzul;
FANTASMA fantasmaLaranja;

void fantasma_init(FANTASMA* fantasma) {

    fantasma->x = 11 * (float)BUFFER_W / TAM_X;
    fantasma->y = 11 * (float)BUFFER_H / TAM_Y;
    fantasma->dy = 1;
    fantasma->speed = 1.0;
    fantasma->timer = 0;
    fantasma->direcao = 0;
}

void verificaColisao(FANTASMA fantasma, FANTASMA* fantasmaP) {

    int prox_x, prox_y, x, y;
    prox_x = (fantasma.x + fantasma.dx) / ((float)BUFFER_W / TAM_X);
    prox_y = (fantasma.y + fantasma.dy) / ((float)BUFFER_H / TAM_Y);
    x = (int)(fantasma.x + fantasma.dx + 10) / ((float)BUFFER_W / TAM_X);
    y = (int)(fantasma.y + fantasma.dy + 10) / ((float)BUFFER_H / TAM_Y);

    if (mapa[prox_y][prox_x] == 1 || mapa[y][x] == 1 || mapa[prox_y][x] == 1 || mapa[y][prox_x] == 1)
    {
        fantasmaP->timer = 50;
        fantasmaP->dx = 0;
        fantasmaP->dy = 0;
    }
}

void fantasma_update(FANTASMA* fantasma)
{
    verificaColisao(*fantasma, fantasma);
    if (fantasma->timer == 50) {
        // Gera um número aleatório entre 0 e 3 para representar as direções possíveis
        int direcao = rand() % 4;

        // Evita que a mesma direção seja escolhida consecutivamente
        while (fantasma->direcao == direcao) {
            direcao = rand() % 4;
        }
        fantasma->direcao = direcao;
        fantasma->timer = 0;
    }

    fantasma->x += fantasma->dx;
    fantasma->y += fantasma->dy;

    // Atualiza a posição do fantasma com base na direção gerada aleatoriamente
    switch (fantasma->direcao) {
    case 0: // Mover para cima
        fantasma->dy = -fantasma->speed;
        fantasma->dx = 0;
        fantasma->posit = 2;
        break;
    case 1: // Mover para baixo
        fantasma->dy = fantasma->speed;
        fantasma->dx = 0;
        fantasma->posit = 3;
        break;
    case 2: // Mover para a esquerda
        fantasma->dx = -fantasma->speed;
        fantasma->dy = 0;
        fantasma->posit = 1;
        break;
    case 3: // Mover para a direita
        fantasma->dx = fantasma->speed;
        fantasma->dy = 0;
        fantasma->posit = 0;
        break;
    }

}

void fantasma_draw() {
    al_draw_bitmap(sprites.fantasmaVermelho[fantasmaVermelho.posit], fantasmaVermelho.x, fantasmaVermelho.y, 0);
    al_draw_bitmap(sprites.fantasmaRosa[fantasmaRosa.posit], fantasmaRosa.x, fantasmaRosa.y, 0);
    al_draw_bitmap(sprites.fantasmaAzul[fantasmaAzul.posit], fantasmaAzul.x, fantasmaAzul.y, 0);
    al_draw_bitmap(sprites.fantasmaLaranja[fantasmaLaranja.posit], fantasmaLaranja.x, fantasmaLaranja.y, 0);
}

// ------------ PACMAN ------------

#define PACMAN_SPEED 1
#define PACMAN_MAX_X (BUFFER_W - PACMAN_W)
#define PACMAN_MAX_Y (BUFFER_H - PACMAN_H)

typedef struct PACMAN
{
    int x, y;
    int dx, dy;
    int lives;
    int respawn_timer;
    int invincible_timer;
    int pos;
    int ponts;
} PACMAN;
PACMAN pacman;

void pacman_reset() {
    pacman.x = 14 * (float)BUFFER_W / TAM_X;
    pacman.y = 17 * (float)BUFFER_H / TAM_Y;
    pacman.dx = 0;
    pacman.dy = 0;
    pacman.invincible_timer = 120;
    fantasmaAzul.speed++;
    fantasmaLaranja.speed++;
    fantasmaRosa.speed++;
}
void pacman_init()
{
    pacman.x = 11 * (float)BUFFER_W / TAM_X;
    pacman.y = 17 * (float)BUFFER_H / TAM_Y;
    pacman.dx = 0;
    pacman.dy = 0;
    pacman.lives = 3;
    pacman.respawn_timer = 0;
    pacman.invincible_timer = 120;
    pacman.pos = 0;
    pacman.ponts = 0;
}

void verificaPosicao() {

    int prox_x, prox_y, x, y;
    prox_x = (int) (pacman.x + pacman.dx) / ((float)BUFFER_W / TAM_X);
    prox_y = (int) (pacman.y + pacman.dy) / ((float)BUFFER_H / TAM_Y);
    x = (int)(pacman.x + pacman.dx + 10) / ((float)BUFFER_W / TAM_X);
    y = (int)(pacman.y + pacman.dy + 10) / ((float)BUFFER_H / TAM_Y);

    if (mapa[prox_y][prox_x] == 1 || mapa[y][x] == 1 || mapa[prox_y][x] == 1 || mapa[y][prox_x] == 1)
    {
        printf("[%d,%d][%d,%d]", pacman.x, pacman.y, prox_x, prox_y);
        pacman.dx = 0;
        pacman.dy = 0;
    }

}

void verificaPontos() {
    //Calcular os pontos do pacman
    int x, y;
    x = (pacman.x) / ((float)BUFFER_W / TAM_X);
    y = (pacman.y) / ((float)BUFFER_H / TAM_Y);
    if (mapa[y][x] == 2) {
        pacman.ponts++;
        mapa[y][x] = 0;
    }
    else if (mapa[y][x] == 3) {
        pacman.ponts++;
        mapa[y][x] = 0;
        pacman.invincible_timer = 360;
    }
}

void colisaoComFantasmas(FANTASMA fantasma) {

    int pacmanX, pacmanY, fantasmaX, fantasmaY;
    pacmanX = (pacman.x +6) / ((float)BUFFER_W / TAM_X);
    pacmanY = (pacman.y +6) / ((float)BUFFER_H / TAM_Y);
    fantasmaX = (fantasma.x +6) / ((float)BUFFER_W / TAM_X);
    fantasmaY = (fantasma.y +6) / ((float)BUFFER_H / TAM_Y);

    if (pacmanX == fantasmaX && pacmanY == fantasmaY) {
        pacman.lives--;
        pacman_reset();
    }
}

void pacman_update()
{
    if (pacman.lives < 0)
        return;

    if (pacman.respawn_timer)
    {
        pacman.respawn_timer--;
        return;
    }

    pacman.x += pacman.dx;
    pacman.y += pacman.dy;

    if (key[ALLEGRO_KEY_LEFT])
    {
        pacman.dx = -PACMAN_SPEED;
        pacman.dy = 0;
        pacman.pos = 1;
    }
    if (key[ALLEGRO_KEY_RIGHT])
    {
        pacman.dx = PACMAN_SPEED;
        pacman.dy = 0;
        pacman.pos = 0;
    }
    if (key[ALLEGRO_KEY_UP])
    {
        pacman.dy = -PACMAN_SPEED;
        pacman.dx = 0;
        pacman.pos = 2;
    }
    if (key[ALLEGRO_KEY_DOWN])
    {
        pacman.dy = PACMAN_SPEED;
        pacman.dx = 0;
        pacman.pos = 3;
    }

    // verifica colisao
    verificaPosicao();

    // verifica pontos
    verificaPontos();

    if (pacman.invincible_timer)
        pacman.invincible_timer--;
    else
    {   
        colisaoComFantasmas(fantasmaVermelho);
        colisaoComFantasmas(fantasmaRosa);
        colisaoComFantasmas(fantasmaAzul);
        colisaoComFantasmas(fantasmaLaranja);
    }
}

void pacman_draw()
{
    if (pacman.lives < 0)
        return;
    if (pacman.respawn_timer)
        return;
    if (((pacman.invincible_timer / 2) % 3) == 1)
        return;

    // al_draw_bitmap(sprites.pacman[pacman.pos], pacman.x * (float)BUFFER_W / TAM_X, pacman.y * (float)BUFFER_H / TAM_Y, 0);
    al_draw_bitmap(sprites.pacman[pacman.pos], pacman.x, pacman.y, 0);
}

// ---------- Fantasma inteligente ----------
/*
typedef struct vertex {
    int index;
    int distance;
    int parent;
} vertex;

void init_adjacency_matrix(vertex* adjacency_matrix, int size) {
    for (int i = 0; i < size; i++) {
        adjacency_matrix[i].index = i;
        adjacency_matrix[i].distance = INT_MAX;
        adjacency_matrix[i].parent = -1;
    }
}

int calculate_distance(vertex* adjacency_matrix, int source, int destination) {
    // Implemente sua função para calcular a distância entre dois vértices aqui.
}

void dijkstra(vertex* adjacency_matrix, int source, int size) {
    adjacency_matrix[source].distance = 0;

    for (int i = 0; i < size; i++) {
        int current_vertex = -1;
        int min_distance = INT_MAX;

        for (int j = 0; j < size; j++) {
            if (adjacency_matrix[j].distance < min_distance && !adjacency_matrix[j].visited) {
                current_vertex = j;
                min_distance = adjacency_matrix[j].distance;
            }
        }

        adjacency_matrix[current_vertex].visited = 1;

        for (int j = 0; j < size; j++) {
            if (!adjacency_matrix[j].visited && calculate_distance(adjacency_matrix, current_vertex, j) < adjacency_matrix[j].distance) {
                adjacency_matrix[j].distance = calculate_distance(adjacency_matrix, current_vertex, j);
                adjacency_matrix[j].parent = current_vertex;
            }
        }
    }
}
int calculate_distance(vertex* adjacency_matrix, int source, int destination) {
    // Implemente sua função para calcular a distância entre dois vértices aqui.

    // O movimento necessário para chegar à célula destino é o índice do vértice pai da célula destino.
    int movement = adjacency_matrix[destination].parent;

    return movement;
}
// Esquerda 0
// Direita 2
// Cima 1
// Baixo 3

void fantasma_vermelho_atuzlizar() {
    int prox_x, prox_y, x, y;
    prox_x = (fantasmaVermelho.x + fantasmaVermelho.dx) / ((float)BUFFER_W / TAM_X);
    prox_y = (fantasmaVermelho.y + fantasmaVermelho.dy) / ((float)BUFFER_H / TAM_Y);
    x = (int)(fantasmaVermelho.x + fantasmaVermelho.dx + 10) / ((float)BUFFER_W / TAM_X);
    y = (int)(fantasmaVermelho.y + fantasmaVermelho.dy + 10) / ((float)BUFFER_H / TAM_Y);

    if (mapa[prox_y][prox_x] == 1 || mapa[y][x] == 1 || mapa[prox_y][x] == 1 || mapa[y][prox_x] == 1)
    {
        fantasmaVermelho.dx = 0;
        fantasmaVermelho.dy = 0;
    }

    if (fantasmaVermelho.x < pacman.x && fantasmaVermelho.direcao != 0 )
    {
        fantasmaVermelho.direcao = 2;
    }
    else if (fantasmaVermelho.x > pacman.x && fantasmaVermelho.direcao != 2)
    {
        fantasmaVermelho.direcao = 0;
    }
    else if (fantasmaVermelho.y < pacman.y && fantasmaVermelho.direcao != 1)
    {
        fantasmaVermelho.direcao = 3;
    }
    else if (fantasmaVermelho.y > pacman.y && fantasmaVermelho.direcao != 3 )
    {
        fantasmaVermelho.direcao = 1;
    }

    fantasmaVermelho.x += fantasmaVermelho.dx;
    fantasmaVermelho.y += fantasmaVermelho.dy;

    switch (fantasmaVermelho.direcao) {
    case 1: // Mover para cima
        fantasmaVermelho.dy = -fantasmaVermelho.speed;
        fantasmaVermelho.dx = 0;
        fantasmaVermelho.posit = 2;
        break;
    case 3: // Mover para baixo
        fantasmaVermelho.dy = fantasmaVermelho.speed;
        fantasmaVermelho.dx = 0;
        fantasmaVermelho.posit = 3;
        break;
    case 0: // Mover para a esquerda
        fantasmaVermelho.dx = -fantasmaVermelho.speed;
        fantasmaVermelho.dy = 0;
        fantasmaVermelho.posit = 1;
        break;
    case 2: // Mover para a direita
        fantasmaVermelho.dx = fantasmaVermelho.speed;
        fantasmaVermelho.dy = 0;
        fantasmaVermelho.posit = 0;
        break;
    }
}
*/

// ---------- MAIN ----------

int main()
{
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    disp_init();

    //desenhar o fundo com uma imagem
    //fundo = al_load_bitmap("../sprites/map/map.png", NULL);
    //must_init(fundo, "fundo");

    // audio_init();

    must_init(al_init_image_addon(), "image");
    sprites_init();

    // hud_init();

    must_init(al_init_primitives_addon(), "primitives");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    /*----- Inicio das funções -----*/
    keyboard_init();
    pacman_init();
    fantasma_init(&fantasmaVermelho);
    fantasma_init(&fantasmaRosa);
    fantasma_init(&fantasmaAzul);
    fantasma_init(&fantasmaLaranja);
    mapa_draw();

    frames = 0;
    score = 0;

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);
    while (1)
    {
        al_wait_for_event(queue, &event);

        switch (event.type)
        {
        case ALLEGRO_EVENT_TIMER:
            pacman_update();
            fantasma_update(&fantasmaVermelho);
            //fantasma_vermelho_atuzlizar();
            fantasma_update(&fantasmaRosa);
            fantasma_update(&fantasmaAzul);
            fantasma_update(&fantasmaLaranja);

            if (key[ALLEGRO_KEY_ESCAPE])
                done = true;

            redraw = true;
            frames++;
            break;

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            done = true;
            break;
        }

        if (done)
            break;

        keyboard_update(&event);

        if (redraw && al_is_event_queue_empty(queue))
        {
            disp_pre_draw();
            al_clear_to_color(al_map_rgb(0, 0, 0));

            /*----- Desenhar -----*/

            mapa_draw();
            pacman_draw();
            fantasma_draw();

            /*----- Acrescentar o tempo -----*/
            fantasmaVermelho.timer += 1;
            fantasmaRosa.timer += 1;
            fantasmaAzul.timer += 1;
            fantasmaLaranja.timer += 1;




            disp_post_draw();
            redraw = false;
        }

        if (pacman.ponts >= 244) {//244 pontos no total
            break;
        }
    }
    al_destroy_bitmap(fundo);
    sprites_deinit();
    disp_deinit();
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
