#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>


long frames;
long score;

/*
must_init es una funcion que asegura que cada paso se ejecuto de manera correcta,
si no lo hizo entonces lo inprime en consola
*/
void must_init(bool test, const char* description)
{
    if (test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

//constantes del display
#define BUFFER_W 352
#define BUFFER_H 272

int DISP_SCALE = 3;
#define DISP_W (BUFFER_W * DISP_SCALE)
#define DISP_H (BUFFER_H * DISP_SCALE)

ALLEGRO_DISPLAY* disp;
ALLEGRO_BITMAP* buffer;

//funcion que crean el display

void disp_init()
{
    disp = al_create_display(DISP_W, DISP_H);
    must_init(disp, "display");

    buffer = al_create_bitmap(BUFFER_W, BUFFER_H);
    must_init(buffer, "bitmap buffer");
}

//funcion que destruye el display y el bitmap

void disp_deinit()
{
    al_destroy_bitmap(buffer);
    al_destroy_display(disp);
}

//funcion para mostrar cosas en el buffer en vez del display
void disp_pre_draw()
{
    al_set_target_bitmap(buffer);
}

//funcion para dibujar otra vez en el display en vez de el buffer
void disp_post_draw()
{
    al_set_target_backbuffer(disp);
    al_draw_scaled_bitmap(buffer, 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISP_W, DISP_H, 0);

    al_flip_display();
}

#define KEY_SEEN     1
#define KEY_RELEASED 2
unsigned char key[ALLEGRO_KEY_MAX];

void keyboard_init()
{
    memset(key, 0, sizeof(key));
}

//funcion que se llamara cada loop para asegurar que las teclas se actualizan cada vez
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

#define SNAKE_W 16
#define SNAKE_H 16

#define APPLE_W 32
#define APPLE_H 16

#define GRASS_W 15
#define GRASS_H 31

#define WEED_W 31
#define WEED_H 31


    typedef struct SPRITES
    {
        ALLEGRO_BITMAP* _sheet;

        ALLEGRO_BITMAP* snake;

        ALLEGRO_BITMAP* apple;

        ALLEGRO_BITMAP* grass;
        ALLEGRO_BITMAP* weed;

        ALLEGRO_BITMAP* menu;

        ALLEGRO_BITMAP* lose;

    } SPRITES;
    SPRITES sprites;

ALLEGRO_BITMAP* sprite_grab(int x, int y, int w, int h)
{
    ALLEGRO_BITMAP* sprite = al_create_sub_bitmap(sprites._sheet, x, y, w, h);
    must_init(sprite, "sprite grab");
    return sprite;
}

//se declaran los pixeles de los sprites
void sprites_init()
{
    sprites._sheet = al_load_bitmap("GameSnake.png");
    must_init(sprites._sheet, "GameSnake");

    sprites.snake = sprite_grab(0, 0, SNAKE_W, SNAKE_H);

    sprites.apple = sprite_grab(16, 0, APPLE_W, APPLE_H);

    sprites.grass = sprite_grab(0, 17, GRASS_W, GRASS_H);
    sprites.weed = sprite_grab(16, 17, WEED_W, WEED_H);
}

void sprites_deinit()
{
    al_destroy_bitmap(sprites.snake);
    al_destroy_bitmap(sprites.apple);
    al_destroy_bitmap(sprites.grass);
    al_destroy_bitmap(sprites.weed);

    al_destroy_bitmap(sprites._sheet);
}

void screen_deinit()
{
    al_destroy_bitmap(sprites.menu);
    al_destroy_bitmap(sprites.lose);
}

#define PLAY_Y 100
#define OPTIONS_Y 132
#define SCORE_Y 164
#define CREDITS_Y 196
#define CLOSE_Y 228

ALLEGRO_BITMAP* menu = al_load_bitmap("imagenes/menu/menu_empty.png");

ALLEGRO_FONT* fontm;

void menu_screen_init()
{
    sprites.menu = al_load_bitmap("imagenes/menu/menu_empty.png");
    must_init(sprites.menu, "menu");

    fontm = al_create_builtin_font();
    must_init(fontm, "font");
}

#define PLAY 1
#define OPTIONS 2
#define SCORE 3
#define CREDITS 4
#define CLOSE 5

int mainmenu = 1;
int menu_selection = 0;

int r1 = 1, r2 = 1, r3 = 1, r4 = 1, r5 = 1;
int g1 = 1, g2 = 1, g3 = 1, g4 = 1, g5 = 1;
int b1 = 1, b2 = 1, b3 = 1, b4 = 1, b5 = 1;

void menu_update()
{

    if (key[ALLEGRO_KEY_UP])
    {
        if (mainmenu != PLAY)
        {
            mainmenu--;
        }
    }
    if (key[ALLEGRO_KEY_DOWN])
    {
        if (mainmenu != CLOSE)
        {
            mainmenu++;
        }
    }

    if (mainmenu == PLAY)
    {
        b1 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {
            menu_selection = 1;
        }
    }
    else
    {
        b1 = 1;
    }

    if (mainmenu == OPTIONS)
    {
        b2 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {
            menu_selection = 2;
        }
    }
    else
    {
        b2 = 1;
    }

    if (mainmenu == SCORE)
    {
        b3 = 0;
    }
    else
    {
        b3 = 1;
    }

    if (mainmenu == CREDITS)
    {
        b4 = 0;
    }
    else
    {
        b4 = 1;
    }

    if (mainmenu == CLOSE)
    {
        b5 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {
            menu_selection = 5;
        }
    }
    else
    {
        b5 = 1;
    }
}

void menu_draw()    
{

    al_draw_bitmap(sprites.menu, 0, 0, 0);
    al_draw_textf(fontm, al_map_rgb_f(r1, g1, b1), 16, PLAY_Y, 0, "PLAY");
    al_draw_textf(fontm, al_map_rgb_f(r2, g2, b2), 16, OPTIONS_Y, 0, "OPTIONS");
    al_draw_textf(fontm, al_map_rgb_f(r3, g3, b3), 16, SCORE_Y, 0, "SCORE");
    al_draw_textf(fontm, al_map_rgb_f(r4, g4, b4), 16, CREDITS_Y, 0, "CREDITS");
    al_draw_textf(fontm, al_map_rgb_f(r5, g5, b5), 16, CLOSE_Y, 0, "CLOSE GAME");
}

ALLEGRO_BITMAP* lose = al_load_bitmap("imagenes/lose/you_died.png");
ALLEGRO_FONT* fontl;

#define RESTART_Y 164
#define LOSE_CLOSE_Y 196

void lose_screen_init()
{
    sprites.lose = al_load_bitmap("imagenes/lose/you_died.png");
    must_init(sprites.lose, "lose");
}

#define RESTART 1
#define LOSE_CLOSE 2

int lose_menu = 1;
int lose_menu_selection = 0;

void lose_update()
{

    if (key[ALLEGRO_KEY_UP])
    {
        if (lose_menu != RESTART)
        {
            lose_menu--;
        }
    }
    if (key[ALLEGRO_KEY_DOWN])
    {   
        if (lose_menu != LOSE_CLOSE)
        {
            lose_menu++;
        }
    }
    
    if (lose_menu == RESTART)
    {
        g1 = 0;
        b1 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {
            lose_menu_selection = 1;
        }
    }
    else
    {
        g1 = 1;
        b1 = 1;
    }
    if (lose_menu == LOSE_CLOSE)
    {
        g2 = 0;
        b2 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {
            lose_menu_selection = 2;

        }
    }
    else
    {
        g2 = 1;
        b2 = 1;
    }
}

void lose_draw()
{
    al_draw_bitmap(sprites.lose, 0, 0, 0);
    al_draw_textf(fontm, al_map_rgb_f(r1, g1, b1), 16, RESTART_Y, 0, "RESTART");
    al_draw_textf(fontm, al_map_rgb_f(r2, g2, b2), 16, LOSE_CLOSE_Y, 0, "CLOSE GAME");
}

#define SOUND_Y 100
#define FULLSCREEN_Y 132
#define DALTONIC_Y 164
#define ACCESSIBILITY_Y 196
#define BACK_Y 228

#define SOUND 1
#define FULLSCREEN 2
#define DALTONIC 3
#define ACCESSIBILITY 4
#define BACK 5

int option_menu = 1;
int option_menu_selection = 0;

void options_update()
{

    if (key[ALLEGRO_KEY_UP])
    {
        if (option_menu != SOUND)
        {
            option_menu--;
        }
    }
    if (key[ALLEGRO_KEY_DOWN])
    {
        if (option_menu != BACK)
        {
            option_menu++;
        }
    }

    if (option_menu == SOUND)
    {
        b1 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {
            option_menu_selection = 1;
        }
    }
    else
    {
        b1 = 1;
    }
    if (option_menu == FULLSCREEN)
    {
        b2 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {
            option_menu_selection = 2;
        }
    }
    else
    {
        b2 = 1;
    }

    if (option_menu == DALTONIC)
    {
        b3 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {
            option_menu_selection = 3;
        }
    }
    else
    {
        b3 = 1;
    }

    if (option_menu == ACCESSIBILITY)
    {
        b4 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {
            option_menu_selection = 4;
        }
    }
    else
    {
        b4 = 1;
    }

    if (option_menu == BACK)
    {
        b5 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {
            option_menu_selection = 5;
        }
    }
    else
    {
        b5 = 1;
    }

}

void options_draw()
{
    al_draw_bitmap(sprites.menu, 0, 0, 0);
    al_draw_textf(fontm, al_map_rgb_f(r1, g1, b1), 16, SOUND_Y, 0, "SOUND OPTIONS");
    al_draw_textf(fontm, al_map_rgb_f(r2, g2, b2), 16, FULLSCREEN_Y, 0, "FULLSCREEN MODE");
    al_draw_textf(fontm, al_map_rgb_f(r3, g3, b3), 16, DALTONIC_Y, 0, "DALTONIC MODE");
    al_draw_textf(fontm, al_map_rgb_f(r4, g4, b4), 16, ACCESSIBILITY_Y, 0, "ACCESSIBILITY OPTIONS");
    al_draw_textf(fontm, al_map_rgb_f(r5, g5, b5), 16, BACK_Y, 0, "BACK");
}

float frames_per_second = 10.0;

#define SPEED_Y 132
#define DISPLAY_Y 164

#define SPEED 1
#define DISPLAY 2
#define ACCESSIBILTY_BACK 3

int accessibility_option_menu = 1;
int accessibility_option_menu_selection = 0;

void options_accessibility_update()
{
    if (key[ALLEGRO_KEY_UP])
    {
        if (accessibility_option_menu != SPEED)
        {
            accessibility_option_menu--;
        }
    }
    if (key[ALLEGRO_KEY_DOWN])
    {
        if (accessibility_option_menu != ACCESSIBILTY_BACK)
        {
            accessibility_option_menu++;
        }
    }

    if (accessibility_option_menu == SPEED)
    {
        b1 = 0;
        if (key[ALLEGRO_KEY_RIGHT])
        {
            if (accessibility_option_menu != 100.0)
            {
                frames_per_second+=10;
            }
        }
        if (key[ALLEGRO_KEY_LEFT])
        {
            if (accessibility_option_menu != 1.0)
            {
                frames_per_second--;
            }
        }
    }
    else
    {
        b1 = 1;
    }
    if (accessibility_option_menu == DISPLAY)
    {
        b2 = 0;
        if (key[ALLEGRO_KEY_RIGHT])
        {
            if (accessibility_option_menu != 4.0)
            {
                DISP_SCALE++;
            }
        }
        if (key[ALLEGRO_KEY_LEFT])
        {
            if (accessibility_option_menu != 1.0)
            {
                DISP_SCALE--;
            }
        }
    }
    else
    {
        b2 = 1;
    }

    if (accessibility_option_menu == ACCESSIBILTY_BACK)
    {
        b3 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {
            accessibility_option_menu_selection = 3;
        }
    }
    else
    {
        b3 = 1;
    }
}

void accessibility_options_draw()
{
    al_draw_bitmap(sprites.menu, 0, 0, 0);
    al_draw_textf(fontm, al_map_rgb_f(r1, g1, b1), 16, SPEED_Y, 0, "SNAKE SPEED");
    al_draw_textf(fontm, al_map_rgb_f(r2, g2, b2), 16, DISPLAY_Y, 0, "DISPLAY SCALE");
    al_draw_textf(fontm, al_map_rgb_f(r3, g3, b3), 16, ACCESSIBILITY_Y, 0, "BACK TO OPTIONS");
}

//propiedades de la serpiente
#define SNAKE_MAX_X (BUFFER_W - SNAKE_W)
#define SNAKE_MAX_Y (BUFFER_H - SNAKE_H)
#define LIMIT 300
int tales = 3;

typedef struct SNAKE
{
    int x, y;
    int lives;

}SNAKE;
SNAKE snake[LIMIT];

void snake_init()
{
    snake[0].x = (BUFFER_W / 2) - (SNAKE_W);
    snake[0].y = (BUFFER_W / 2) - (SNAKE_H);
    snake[0].lives = 0;

    
    for (int i = tales + 1; i < LIMIT; i++)
    {
        snake[i] = snake[i - 1];
    }
    
}

int movement = 4;

void snake_update()
{
    if (snake[0].lives < 0)
        return;

    if (key[ALLEGRO_KEY_UP])
    {
        if (movement != 2)
            movement = 8;
    }
    else if (key[ALLEGRO_KEY_DOWN])
    {
        if (movement != 8)
            movement = 2;
    }
    else if (key[ALLEGRO_KEY_LEFT])
    {
        if (movement != 6)
            movement = 4;
    }
    else if (key[ALLEGRO_KEY_RIGHT])
    {
        if (movement != 4)
            movement = 6;
    }

    if (movement == 8) {
        snake[0].y -= 16;
    }
    else if (movement == 2) {
        snake[0].y += 16;
    }
    else if (movement == 4) {
        snake[0].x -= 16;
    }
    else if (movement == 6) {
        snake[0].x += 16;
    }

    if (snake[0].x < 16)
        snake[0].x = 16;
    if (snake[0].y < 16)
        snake[0].y = 16;

    if (snake[0].x > SNAKE_MAX_X - 16)
        snake[0].x = SNAKE_MAX_X - 16;
    if (snake[0].y > SNAKE_MAX_Y - 16)
        snake[0].y = SNAKE_MAX_Y - 16;


    for (int i = 1; i < LIMIT; i++)
    {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
        {
            snake[0].lives--;
        }
    }


    for (int i = tales + 1; i < LIMIT; i++)
    {
        snake[i] = snake[i - 1];
    }

}

void snake_draw()
{
    if (snake[0].lives < 0)
    {
        menu_selection = 6;
        lose_update();
        lose_draw();
        return;
    }

    al_draw_bitmap(sprites.snake, snake[0].x, snake[0].y, 0);
}


void tale_init()
{
    for (int i = 1; i <= tales; i++)
    {
        snake[i].x = snake[0].x + (i * 16);
        snake[i].y = snake[0].y;
    }
}

void tale_update()
{
    for (int i = tales; i > 0; i--)
    {
        snake[i] = snake[i - 1];
    }
}

void tale_draw()
{
    if (snake[0].lives < 0)
        return;

    for (int i = 1; i <= tales; i++)
    {
        al_draw_bitmap(sprites.snake, snake[i].x, snake[i].y, 0);
    }
}

typedef struct APPLE
{
    int x, y;

}APPLE;
APPLE apple;

int xVector[20];
int yVector[15];
int aux = 16, auxi = 16;
int xRand, yRand;

void apple_init()
{

    for (int i = 0; i < 20; i++)
    {
        xVector[i] = aux;
        aux += 16;
    }

    for (int i = 0; i < 15; i++)
    {
        yVector[i] = auxi;
        auxi += 16;
    }

    xRand = rand() % 20;
    yRand = rand() % 15;

    apple.x = xVector[xRand];
    apple.y = yVector[yRand];
}

void apple_update()
{
    int overlap = 0;

    if (snake[0].x == apple.x && snake[0].y == apple.y)
    {
        tales += 1;
        score += 50;

        xRand = rand() % 20;
        yRand = rand() % 15;

        apple.x = xVector[xRand];
        apple.y = yVector[yRand];

        while (overlap == 0)
        {
            for (int i = 0; i < 300; i++)
            {
                if (apple.x == snake[i].x && apple.y == snake[i].y)
                {
                    xRand = rand() % 20;
                    yRand = rand() % 15;

                    apple.x = xVector[xRand];
                    apple.y = yVector[yRand];
                }
                else
                {
                    overlap = 1;
                }
            }
        }
    }
}

void apple_draw()
{
    al_draw_bitmap(sprites.apple, apple.x, apple.y, 0);
}

ALLEGRO_FONT* font;
long score_display;

void hud_init()
{
    font = al_create_builtin_font();
    must_init(font, "font");

    score_display = 0;
}

void hud_deinit()
{
    al_destroy_font(font);
}

void hud_update()
{
    if (frames % 2)
        return;

    for (long i = 5; i > 0; i--)
    {
        long diff = 1 << i;
        if (score_display <= (score - diff))
            score_display += diff;
    }
}

void hud_draw()
{
    al_draw_textf(
        font,
        al_map_rgb_f(1, 1, 1),
        1, 1,
        0,
        "%05ld",
        score_display
    );
}

void framework_draw()
{
    al_draw_filled_rectangle(0, 0, 352, 272, al_map_rgba_f(0.6, 0.6, 0.6, 0));
    al_draw_filled_rectangle(16, 16, 336, 256, al_map_rgba_f(0, 0.5, 0, 0));
}

void restart()
{
    menu_selection = 1;

    score = 0;
    score_display = 0;
    tales = 3;
    movement = 4;

    snake_init();
    tale_init();

    apple.x = xVector[rand() % 20];
    apple.y = yVector[rand() % 15];

    int overlap = 0;

    overlap = 0;

    while (overlap == 0)
    {
        for (int i = 0; i < 300; i++)
        {
            if (apple.x == snake[i].x && apple.y == snake[i].y)
            {
                xRand = rand() % 20;
                yRand = rand() % 15;

                apple.x = xVector[xRand];
                apple.y = yVector[yRand];
            }
            else
            {
                overlap = 1;
            }
        }
    }

    lose_menu_selection = 0;
}

int main()
{
    srand(time(NULL));

    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_init_primitives_addon(), "primitives");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / frames_per_second);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");



    disp_init();

    must_init(al_init_image_addon(), "image");
    sprites_init();
    menu_screen_init();
    lose_screen_init();


    hud_init();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    keyboard_init();
    snake_init();
    tale_init();
    apple_init();

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

            if (menu_selection == 0)
            {
                menu_update();
            }

            if (menu_selection == 1)
            {
                tale_update();
                snake_update();
                apple_update();
                hud_update();
            }

            if (menu_selection == 2)
            {
                mainmenu = 6;
                options_update();
                if (option_menu_selection == 4)
                {
                    options_accessibility_update();
                }

                if (option_menu_selection == 5)
                {
                    mainmenu = 1;
                    option_menu = 1;
                    menu_selection = 0;
                    option_menu_selection = 0;
                    break;
                }
            }

            if (lose_menu_selection == 1)
            {
                restart();
            }

            if (menu_selection == 5)
            {
                done = true;
            }

            if (lose_menu_selection == 2)
            {
                done = true;
            }

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
            
            framework_draw();

            apple_draw();
            tale_draw();
            hud_draw();
            snake_draw();

            if (menu_selection == 0)
            {
                menu_draw();
            }

            if (menu_selection == 2)
            {
                options_draw();
            }

            if (option_menu_selection == 4)
            {
                accessibility_options_draw();
            }

            disp_post_draw();
            redraw = false;
        }
    }

    sprites_deinit();
    screen_deinit();
    disp_deinit();
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    hud_deinit();

    return 0;
}