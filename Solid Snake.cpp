#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <ctime>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>


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

void delay(int secs)
{
    for (int i = (time(NULL) + secs); time(NULL) != i; time(NULL));
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

        ALLEGRO_BITMAP* body_c;
        ALLEGRO_BITMAP* body_f;

        ALLEGRO_BITMAP* head_up;
        ALLEGRO_BITMAP* head_down;
        ALLEGRO_BITMAP* head_right;
        ALLEGRO_BITMAP* head_left;

        ALLEGRO_BITMAP* corner_tl;
        ALLEGRO_BITMAP* corner_tr;
        ALLEGRO_BITMAP* corner_bl;
        ALLEGRO_BITMAP* corner_br;

        ALLEGRO_BITMAP* apple;

        ALLEGRO_BITMAP* grass;
        ALLEGRO_BITMAP* weed;

        ALLEGRO_BITMAP* menu;
        ALLEGRO_BITMAP* credits;

        ALLEGRO_BITMAP* framework;
        ALLEGRO_BITMAP* loading;

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
    sprites._sheet = al_load_bitmap("imagenes/snake/all.png");
    must_init(sprites._sheet, "all");

    sprites.apple = sprite_grab(0, 0, SNAKE_W, SNAKE_H);

    sprites.body_f = sprite_grab(48, 32, SNAKE_W, SNAKE_H);
    sprites.body_c = sprite_grab(32, 32, SNAKE_W, SNAKE_H);

    sprites.head_up = sprite_grab(32, 0, SNAKE_W, SNAKE_H);
    sprites.head_down = sprite_grab(48, 16, SNAKE_W, SNAKE_H);
    sprites.head_right = sprite_grab(48, 0, SNAKE_W, SNAKE_H);
    sprites.head_left = sprite_grab(32, 16, SNAKE_W, SNAKE_H);

    sprites.corner_tl = sprite_grab(0, 32, SNAKE_W, SNAKE_H);
    sprites.corner_tr = sprite_grab(16, 32, SNAKE_W, SNAKE_H);
    sprites.corner_bl = sprite_grab(0, 48, SNAKE_W, SNAKE_H);
    sprites.corner_br = sprite_grab(16, 48, SNAKE_W, SNAKE_H);
}

void sprites_deinit()
{
    al_destroy_bitmap(sprites.snake);
    al_destroy_bitmap(sprites.apple);

    al_destroy_bitmap(sprites.head_up);
    al_destroy_bitmap(sprites.head_down);
    al_destroy_bitmap(sprites.head_right);
    al_destroy_bitmap(sprites.head_left);

    al_destroy_bitmap(sprites.corner_tl);
    al_destroy_bitmap(sprites.corner_tr);
    al_destroy_bitmap(sprites.corner_bl);
    al_destroy_bitmap(sprites.corner_br);

    al_destroy_bitmap(sprites._sheet);
}

void screen_deinit()
{
    al_destroy_bitmap(sprites.menu);
    al_destroy_bitmap(sprites.lose);
}

#define PLAY_Y 132
#define HOW_TO_PLAY_Y 164
#define CREDITS_Y 196
#define CLOSE_Y 228

ALLEGRO_BITMAP* menu = al_load_bitmap("imagenes/menu/menu.png");
ALLEGRO_BITMAP* framework = al_load_bitmap("imagenes/gameplay/framework.png");
ALLEGRO_BITMAP* loading = al_load_bitmap("imagenes/menu/loading.png");
ALLEGRO_BITMAP* credits = al_load_bitmap("imagenes/menu/credits.png");

ALLEGRO_FONT* fontm;

void menu_screen_init()
{
    sprites.menu = al_load_bitmap("imagenes/menu/menu.png");
    must_init(sprites.menu, "menu");
    sprites.credits = al_load_bitmap("imagenes/menu/credits.png");
    must_init(sprites.credits, "credits");

    sprites.framework = al_load_bitmap("imagenes/gameplay/framework.png");
    must_init(sprites.framework, "framework");

    sprites.loading = al_load_bitmap("imagenes/menu/loading.png");
    must_init(sprites.loading, "loading");

    fontm = al_create_builtin_font();
    must_init(fontm, "font");
}

ALLEGRO_SAMPLE* snake_jazz;
ALLEGRO_SAMPLE* menu_change;
ALLEGRO_SAMPLE* meow;
ALLEGRO_SAMPLE* game;
ALLEGRO_SAMPLE* eat_sound;
ALLEGRO_SAMPLE* hurt;
ALLEGRO_SAMPLE* souls_dead;
ALLEGRO_SAMPLE* guitar;
ALLEGRO_SAMPLE* halo;
ALLEGRO_SAMPLE* hollow_knight;

ALLEGRO_SAMPLE_INSTANCE* main_theme;
ALLEGRO_SAMPLE_INSTANCE* menu_select;
ALLEGRO_SAMPLE_INSTANCE* how_to_play;
ALLEGRO_SAMPLE_INSTANCE* gameplay;
ALLEGRO_SAMPLE_INSTANCE* dead_sound;
ALLEGRO_SAMPLE_INSTANCE* post_dead_sound;
ALLEGRO_SAMPLE_INSTANCE* dead_menu;
ALLEGRO_SAMPLE_INSTANCE* credits_music;

void audio_init()
{
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(16);

    snake_jazz = al_load_sample("music/bso_menu.wav");
    must_init(snake_jazz, "snake jazz");
    main_theme = al_create_sample_instance(snake_jazz);
    must_init(main_theme, "main theme");

    al_set_sample_instance_playmode(main_theme, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(main_theme, al_get_default_mixer());

    hollow_knight = al_load_sample("music/how_to_play.wav");
    must_init(hollow_knight, "hollow_knight");
    how_to_play = al_create_sample_instance(hollow_knight);
    must_init(how_to_play, "main theme");

    al_set_sample_instance_playmode(how_to_play, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(how_to_play, al_get_default_mixer());

    meow = al_load_sample("music/menu_select.wav");
    must_init(meow, "meow");
    menu_select = al_create_sample_instance(meow);
    must_init(menu_select, "menu select");

    al_set_sample_instance_playmode(menu_select, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(menu_select, al_get_default_mixer());

    game = al_load_sample("music/gameplay.wav");
    must_init(game, "game");
    gameplay = al_create_sample_instance(game);
    must_init(gameplay, "gameplay");

    al_set_sample_instance_playmode(gameplay, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(gameplay, al_get_default_mixer());

    hurt = al_load_sample("music/dead_sound.wav");
    must_init(hurt, "hurt");
    dead_sound = al_create_sample_instance(hurt);
    must_init(dead_sound, "dead_sound");

    al_set_sample_instance_playmode(dead_sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(dead_sound, al_get_default_mixer());

    souls_dead = al_load_sample("music/post_dead_sound.wav");
    must_init(souls_dead, "souls dead");
    post_dead_sound = al_create_sample_instance(souls_dead);
    must_init(post_dead_sound, "post_dead_sound");

    al_set_sample_instance_playmode(post_dead_sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(post_dead_sound, al_get_default_mixer());

    guitar = al_load_sample("music/dead_menu.wav");
    must_init(guitar, "guitar");
    dead_menu = al_create_sample_instance(guitar);
    must_init(dead_menu, "dead_menu");

    al_set_sample_instance_playmode(dead_menu, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(dead_menu, al_get_default_mixer());

    halo = al_load_sample("music/credits_music.wav");
    must_init(halo, "halo");
    credits_music = al_create_sample_instance(halo);
    must_init(credits_music, "credits music");

    al_set_sample_instance_playmode(credits_music, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(credits_music, al_get_default_mixer());

    eat_sound = al_load_sample("music/eat_sound.wav");
    must_init(eat_sound, "eat sound");

    menu_change = al_load_sample("music/menu_change.wav");
    must_init(menu_change, "menu change");
}

void audio_deinit()
{
    al_destroy_sample(snake_jazz);
}

#define PLAY 1
#define HOW_TO_PLAY 2
#define CREDITS 3
#define CLOSE 4

int mainmenu = 1;
int menu_selection = 0;

int r1 = 1, r2 = 1, r3 = 1, r4 = 1;
int g1 = 1, g2 = 1, g3 = 1, g4 = 1;
int b1 = 1, b2 = 1, b3 = 1, b4 = 1;

int load = 0;

void menu_update()
{

    if (key[ALLEGRO_KEY_UP])
    {
        if (mainmenu != PLAY)
        {
            al_play_sample(menu_change, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
            mainmenu--;
        }
    }
    if (key[ALLEGRO_KEY_DOWN])
    {
        if (mainmenu != CLOSE)
        {
            al_play_sample(menu_change, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
            mainmenu++;
        }
    }

    if (mainmenu == PLAY)
    {
        b1 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {
            al_play_sample_instance(menu_select);

            menu_selection = 1;
        }
    }
    else
    {
        b1 = 1;
    }

    if (mainmenu == HOW_TO_PLAY)
    {
        b2 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {

            menu_selection = 2;
            al_play_sample_instance(menu_select);
        }
    }
    else
    {
        b2 = 1;
    }

    if (mainmenu == CREDITS)
    {
        b3 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {
            
            menu_selection = 3;
            al_play_sample_instance(menu_select);
        }
    }
    else
    {
        b3 = 1;
    }

    if (mainmenu == CLOSE)
    {
        b4 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {
            al_play_sample_instance(menu_select);
            menu_selection = 5;
        }
    }
    else
    {
        b4 = 1;
    }
}

void menu_draw()    
{

    al_draw_bitmap(sprites.menu, 0, 0, 0);
    al_draw_textf(fontm, al_map_rgb_f(r1, g1, b1), 16, PLAY_Y, 0, "PLAY");
    al_draw_textf(fontm, al_map_rgb_f(r2, g2, b2), 16, HOW_TO_PLAY_Y, 0, "HOW TO PLAY");
    al_draw_textf(fontm, al_map_rgb_f(r3, g3, b3), 16, CREDITS_Y, 0, "CREDITS");
    al_draw_textf(fontm, al_map_rgb_f(r4, g4, b4), 16, CLOSE_Y, 0, "CLOSE GAME");
}

ALLEGRO_BITMAP* lose = al_load_bitmap("imagenes/lose/you_died.png");
ALLEGRO_FONT* fontl;

#define SCORE_Y 132
#define RESTART_Y 164
#define BACK_MENU_Y 196
#define LOSE_CLOSE_Y 228

void lose_screen_init()
{
    sprites.lose = al_load_bitmap("imagenes/lose/you_died.png");
    must_init(sprites.lose, "lose");
}

#define RESTART 1
#define BACK_MENU 2
#define LOSE_CLOSE 3

int lose_menu = 1;
int lose_menu_selection = 0;

void lose_update()
{
    if (key[ALLEGRO_KEY_UP])
    {
        if (lose_menu != RESTART)
        {
            al_play_sample(menu_change, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
            lose_menu--;
        }
    }
    if (key[ALLEGRO_KEY_DOWN])
    {   
        if (lose_menu != LOSE_CLOSE)
        {
            al_play_sample(menu_change, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
            lose_menu++;
        }
    }
    
    if (lose_menu == RESTART)
    {
        g1 = 0;
        b1 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {
            al_play_sample_instance(menu_select);

            lose_menu_selection = 1;
        }
    }
    else
    {
        g1 = 1;
        b1 = 1;
    }

    if (lose_menu == BACK_MENU)
    {
        g2 = 0;
        b2 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {
            al_play_sample_instance(menu_select);

            lose_menu_selection = 2;

            menu_selection = 0;

            g2 = 1;
            b2 = 1;

        }
    }
    else
    {
        g2 = 1;
        b2 = 1;
    }

    if (lose_menu == LOSE_CLOSE)
    {
        g3 = 0;
        b3 = 0;
        if (key[ALLEGRO_KEY_SPACE])
        {
            al_play_sample_instance(menu_select);
            lose_menu_selection = 3;

        }
    }
    else
    {
        g3 = 1;
        b3 = 1;
    }
}

ALLEGRO_FONT* font;
long score_display;

void hud_draw()
{
    al_draw_textf(font, al_map_rgb_f(1, 1, 1), 1, 1, 0, "%05ld", score_display);
}

void lose_draw()
{
    al_draw_bitmap(sprites.lose, 0, 0, 0);
    al_draw_textf(fontm, al_map_rgb_f(1, 0, 0), 116, SCORE_Y, 0, "SCORE: %05ld", score_display);
    al_draw_textf(fontm, al_map_rgb_f(r1, g1, b1), 16, RESTART_Y, 0, "RESTART");
    al_draw_textf(fontm, al_map_rgb_f(r2, g2, b2), 16, BACK_MENU_Y, 0, "BACK TO MENU");
    al_draw_textf(fontm, al_map_rgb_f(r3, g3, b3), 16, LOSE_CLOSE_Y, 0, "CLOSE GAME");
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
    int follow;

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

    if (movement == 8) 
    {
        snake[0].y -= 16;
        
        if (snake[0].y != snake[2].y && snake[0].x != snake[2].x && snake[0].x < snake[2].x)
        {
            snake[1].follow = 4;
        }
        else if (snake[0].y != snake[2].y && snake[0].x != snake[2].x && snake[0].x > snake[2].x)
        {
            snake[1].follow = 3;
        }
        else
        {
            snake[1].follow = 2;
        }
    }

    else if (movement == 2) 
    {
        snake[0].y += 16;
        
        if (snake[0].y != snake[2].y && snake[0].x != snake[2].x && snake[0].x < snake[2].x)
        {
            snake[1].follow = 6;
        }
        else if (snake[0].y != snake[2].y && snake[0].x != snake[2].x && snake[0].x > snake[2].x)
        {
            snake[1].follow = 5;
        }
        else
        {
            snake[1].follow = 2;
        }
    }

    else if (movement == 4) 
    {
        snake[0].x -= 16;
        
        if (snake[0].y != snake[2].y && snake[0].x != snake[2].x && snake[0].y < snake[2].y)
        {
            snake[1].follow = 5;
        }
        else if (snake[0].y != snake[2].y && snake[0].x != snake[2].x && snake[0].y > snake[2].y)
        {
            snake[1].follow = 3;
        }
        else
        {
            snake[1].follow = 1;
        }
    }

    else if (movement == 6) 
    {
        snake[0].x += 16;
        snake[0].follow = 1;
    }
    
    if (movement == 6)
    {
        if (snake[0].y != snake[2].y && snake[0].x != snake[2].x && snake[0].y > snake[2].y)
        {
            snake[1].follow = 4;
        }
        else if (snake[0].y != snake[2].y && snake[0].x != snake[2].x && snake[0].y < snake[2].y)
        {
            snake[1].follow = 6;
        }
        else
        {
            snake[1].follow = 1;
        }
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
            al_play_sample_instance(dead_sound);
            al_play_sample_instance(post_dead_sound);
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
        al_play_sample_instance(dead_menu);
        lose_update();
        lose_draw();
        return;
    }
    else
    {
        al_stop_sample_instance(dead_menu);
    }

    switch (movement)
    {
    case 8:
        al_draw_bitmap(sprites.head_up, snake[0].x, snake[0].y, 0);
        break;
    case 2:
        al_draw_bitmap(sprites.head_down, snake[0].x, snake[0].y, 0);
        break;
    case 6:
        al_draw_bitmap(sprites.head_right, snake[0].x, snake[0].y, 0);
        break;
    case 4:
        al_draw_bitmap(sprites.head_left, snake[0].x, snake[0].y, 0);
        break;
    }
    
}


void tale_init()
{
    for (int i = 1; i <= tales; i++)
    {
        snake[i].x = snake[0].x + (i * 16);
        snake[i].y = snake[0].y;
        snake[i-1].follow = 1;
    }
}

void tale_update()
{
    for (int i = tales; i > 0; i--)
    {
        snake[i] = snake[i - 1];
        snake[i].follow = snake[i - 1].follow;
    }
}

void tale_draw()
{
    if (snake[0].lives < 0)
        return;
    
    for (int i = 1; i <= tales; i++)
    {
        switch (snake[i].follow)
        {
        case 1:
            al_draw_bitmap(sprites.body_f, snake[i].x, snake[i].y, 0);

            break;

        case 2:
            al_draw_bitmap(sprites.body_c, snake[i].x, snake[i].y, 0);

            break;

        case 3:
            al_draw_bitmap(sprites.corner_tl, snake[i].x, snake[i].y, 0);

            break;

        case 4:
            al_draw_bitmap(sprites.corner_tr, snake[i].x, snake[i].y, 0);

            break;

        case 5:
            al_draw_bitmap(sprites.corner_bl, snake[i].x, snake[i].y, 0);

            break;

        case 6:
            al_draw_bitmap(sprites.corner_br, snake[i].x, snake[i].y, 0);

            break;
        }
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
        al_play_sample(eat_sound, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

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


void framework_draw()
{
    al_draw_bitmap(sprites.framework, 0, 0, 0);
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

int credits_menu = 1;
int credits_menu_selection = 0;

void credits_update()
{

    if (key[ALLEGRO_KEY_LEFT])
    {
        if (credits_menu != 1)
        {
            credits_menu--;
        }
    }
    if (key[ALLEGRO_KEY_RIGHT])
    {
        if (credits_menu != 2)
        {
            credits_menu++;
        }
    }

    if (credits_menu == 1)
    {
        if (key[ALLEGRO_KEY_LEFT])
        {
            al_play_sample(meow, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

            credits_menu_selection = 1;
        }
    }

    if (credits_menu == 2)
    {
        if (key[ALLEGRO_KEY_RIGHT])
        {
            al_play_sample(meow, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

            credits_menu_selection = 1;
        }
    }
}

void credits_draw()
{
    al_draw_bitmap(sprites.credits, 0, 0, 0);
    al_draw_textf(fontm, al_map_rgb_f(1, 1, 1), 16, 244, 0, "BACK");
    al_draw_textf(fontm, al_map_rgb_f(1, 1, 1), 300, 244, 0, "NEXT");

}

void load_draw()
{
    al_draw_bitmap(sprites.loading, 0, 0, 0);
    al_draw_textf(fontm, al_map_rgb_f(1, 1, 1), 261, 163, 0, "CONTROLS");
    al_draw_textf(fontm, al_map_rgb_f(1, 1, 1), 290, 244, 0, "BACK");
}

int main()
{
    srand(time(NULL));

    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_init_primitives_addon(), "primitives");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 10.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");



    disp_init();

    audio_init();

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
                al_play_sample_instance(main_theme);
            }
            else
            {
                al_stop_sample_instance(main_theme);
            }

            if (menu_selection == 1)
            {
                al_play_sample_instance(gameplay);
                tale_update();
                snake_update();
                apple_update();
                hud_update();
            }
            else
            {
                al_stop_sample_instance(gameplay);
            }

            if (menu_selection == 2)
            {
                al_play_sample_instance(how_to_play);
                credits_update();

                if (credits_menu_selection == 1)
                {
                    credits_menu_selection = 0;
                    menu_selection = 0;
                    credits_menu = 1;

                }
            }
            else
            {
                al_stop_sample_instance(how_to_play);
            }

            if (menu_selection == 3)
            {
                al_play_sample_instance(credits_music);
                credits_update();

                if (credits_menu_selection == 1)
                {
                    credits_menu_selection = 0;
                    menu_selection = 0;
                    credits_menu = 1;

                }
            }
            else
            {
                al_stop_sample_instance(credits_music);
            }

            if (lose_menu_selection == 1)
            {
                restart();
            }

            if (lose_menu_selection == 2)
            {
                
                score = 0;
                score_display = 0;
                tales = 3;
                movement = 4;

                snake_init();
                tale_init();

                lose_menu_selection = 0;
            }

            if (menu_selection == 5)
            {
                done = true;
            }

            if (lose_menu_selection == 3)
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
                load_draw();
            }

            if (menu_selection == 3)
            {
                credits_draw();
            }


            disp_post_draw();
            redraw = false;
        }
    }

    sprites_deinit();
    screen_deinit();
    disp_deinit();
    audio_deinit();
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    hud_deinit();

    return 0;
}