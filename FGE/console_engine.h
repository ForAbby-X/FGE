/*
//### EXEMPLE ###
int onGameCreate(GameEngine *ge)
{
    clear(ge, colorD(0));

    return 1;
}

int onGameUpdate(GameEngine *ge, float elapsed_time)
{
    for (int y = 0; y < ge->winHeight; y++)
        for (int x = 0; x < ge->winWidth; x++)
            draw(ge, x, y, color(rand() & 255, rand() & 255, rand() & 255, 255));


    return 1;
}

void onGameDestroy(GameEngine *ge)
{
}

int main( int argc, char *argv[] ) 
{
    GameEngine game_engine;
    if (createGameEngine(&game_engine, "PUT TITLE HERE !", 320,  240, 4, 4))
        startGameEngine(&game_engine);
    
    return 0;   
}

*/

#ifndef _CONSOLE_ENGINE
#define _CONSOLE_ENGINE

#include "SDL2/SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct  GameEngine_s    GameEngine;
typedef struct  Keys_s          Keys;
typedef union   Color_u         Color;

//### GameEngine ###
int     createGameEngine(GameEngine *ge, char *title, uint16_t win_size_x, uint16_t win_size_y, uint8_t pix_size_x, uint8_t pix_size_y);
int     startGameEngine(GameEngine *ge);
void    destroyGameEngine(GameEngine *ge);

int     updateGraph(GameEngine *ge);
int     processEvents(GameEngine *ge);
int     processKeyboard(GameEngine *ge);
int     processMouse(GameEngine *ge);

int     onGameCreate(GameEngine *ge);                      //undefined - need to be defined by the user
int     onGameUpdate(GameEngine *ge, float elapsed_time);  //undefined - need to be defined by the user
void    onGameDestroy(GameEngine *ge);                     //undefined - need to be defined by the user

Keys    getKey(GameEngine *ge, SDL_Scancode sc);

Keys    getMouse(GameEngine *ge, uint8_t sc);
int     getMouseX(GameEngine *ge);
int     getMouseY(GameEngine *ge);

void    clear(GameEngine *ge, Color col);
void    draw(GameEngine *ge, uint16_t x, uint16_t y, Color col);
void    drawRect(GameEngine *ge, uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, Color col);

//### Color ###
Color color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
Color colorD(uint32_t d);
Color colorF(float r, float g, float b, float a);

struct Keys_s
{
    uint8_t pressed;
    uint8_t held;
    uint8_t released;
};

struct GameEngine_s
{
    uint16_t        winWidth;
    uint16_t        winHeight;
    uint8_t         pixWidth;
    uint8_t         pixHeight;
    char            *title;

    SDL_Window      *window;
    SDL_Renderer    *renderer;
    SDL_Texture     *texture;
    Color           *pixels;

    uint8_t         old_keyboard_keys[256];
    uint32_t        old_mouse_keys;
    Keys            keyboard[256];
    Keys            mouse[5];

    int             mouseX;
    int             mouseY;
};


union Color_u
{
    uint32_t d;
    struct { uint8_t a; uint8_t b; uint8_t g; uint8_t r; };
};

#endif
