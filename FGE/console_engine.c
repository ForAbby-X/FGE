#include "console_engine.h"

//### GameEngine ###
int createGameEngine(GameEngine *ge, char *title,uint16_t win_size_x, uint16_t win_size_y, uint8_t pix_size_x, uint8_t pix_size_y)
{
    ge->winWidth    = win_size_x;
    ge->winHeight   = win_size_y;
    ge->pixWidth    = pix_size_x;
    ge->pixHeight   = pix_size_y;
    ge->title       = title;
    memset(ge->keyboard, 0, 256 * sizeof(Keys));
    memset(ge->mouse, 0, 5 * sizeof(Keys));

    uint16_t real_size_x = win_size_x * pix_size_x;
    uint16_t real_size_y = win_size_y * pix_size_y;
    
    SDL_Init(SDL_INIT_VIDEO);

    ge->window = SDL_CreateWindow(title, 100, 100, real_size_x, real_size_y, 0); 
    if (ge->window == NULL)
    {   
        printf("Could not create window: %s\n", SDL_GetError());
        return -1;
    }
    
    ge->renderer = SDL_CreateRenderer(ge->window, -1, SDL_RENDERER_ACCELERATED);
    if (ge->renderer == NULL)
    {   
        printf("Could not create renderer: %s\n", SDL_GetError());
        return -1;
    }
    
    ge->texture = SDL_CreateTexture(ge->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, win_size_x, win_size_y);
    if (ge->texture == NULL)
    {   
        printf("Could not create texture: %s\n", SDL_GetError());
        return -1;
    }
    
    ge->pixels = (Color*)malloc(win_size_x * win_size_y * sizeof(Color));
    if (ge->pixels == NULL)
    {   
        printf("Could not create pixels data: (malloc fail)\n");
        return -1;
    }

    return 1;
}

int startGameEngine(GameEngine *ge)
{
    int is_running = onGameCreate(ge);
    
    
    uint32_t startTime = 0;
    uint32_t currTime = 0;
    double elapsed_time = 0.0;

    while (is_running)
    {
        startTime = SDL_GetTicks();

        is_running &= processEvents(ge);
        is_running &= processKeyboard(ge);
        is_running &= processMouse(ge);
        is_running &= onGameUpdate(ge, elapsed_time);
        is_running &= updateGraph(ge);
        
        currTime = SDL_GetTicks();
        elapsed_time = (currTime - startTime) / 1000.0;
    }
    
    onGameDestroy(ge);
    destroyGameEngine(ge);

    return 1;
}

void destroyGameEngine(GameEngine *ge)
{
    free(ge->pixels);
    SDL_DestroyTexture(ge->texture);
    SDL_DestroyRenderer(ge->renderer);
    SDL_DestroyWindow(ge->window);

    free(ge);

    SDL_Quit(); 
}

int updateGraph(GameEngine *ge)
{
    int flag = 0;
    flag |= SDL_UpdateTexture(ge->texture, NULL, ge->pixels, ge->winWidth * sizeof(Color));
    flag |= SDL_RenderClear(ge->renderer);
    flag |= SDL_RenderCopy(ge->renderer, ge->texture, NULL, NULL);
    SDL_RenderPresent(ge->renderer);

    return !flag;
}

int processEvents(GameEngine *ge)
{   
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_WINDOWEVENT:
                if ( event.window.event == SDL_WINDOWEVENT_CLOSE )
                {
                    return 0;
                }
            break;
        }
    }

    return 1;
}

int processKeyboard(GameEngine *ge)
{
    uint8_t *new_keys = (uint8_t*)SDL_GetKeyboardState(NULL);

    for (uint32_t i = 0; i < 256; i++)
	{
		ge->keyboard[i].pressed = 0;
		ge->keyboard[i].released = 0;
		if (new_keys[i] != ge->old_keyboard_keys[i])
		{
			if (new_keys[i])
			{
				ge->keyboard[i].pressed = !ge->keyboard[i].held;
				ge->keyboard[i].held = 1;
			}
			else
			{
				ge->keyboard[i].released = 1;
				ge->keyboard[i].held = 0;
			}
		}
		ge->old_keyboard_keys[i] = new_keys[i];
	}

    return 1;
}

int processMouse(GameEngine *ge)
{
    uint32_t new_keys = SDL_GetMouseState(&ge->mouseX, &ge->mouseY);
    ge->mouseX /= ge->pixWidth;
    ge->mouseY /= ge->pixHeight;

    for (uint16_t i = 0; i < 5; i++)
	{
		ge->mouse[i].pressed = 0;
		ge->mouse[i].released = 0;
		if ((new_keys & (1 << i)) != (ge->old_mouse_keys & (1 << i)))
		{
			if (new_keys & (1 << i))
			{
				ge->mouse[i].pressed = !ge->mouse[i].held;
				ge->mouse[i].held = 1;
			}
			else
			{
				ge->mouse[i].released = 1;
				ge->mouse[i].held = 0;
			}
		}
	}
    ge->old_mouse_keys = new_keys;

    return 1; 
}

Keys getKey(GameEngine *ge, SDL_Scancode sc)
{
    return ge->keyboard[sc];
}

Keys getMouse(GameEngine *ge, uint8_t sc)
{
    return ge->mouse[sc];
}


int getMouseX(GameEngine *ge)
{
    return ge->mouseX;
}

int getMouseY(GameEngine *ge)
{
    return ge->mouseY;
}

void clear(GameEngine *ge, Color col)
{
    for (uint32_t i = 0; i < ge->winWidth * ge->winHeight; i++)
        ge->pixels[i] = col;
}

void draw(GameEngine *ge, uint16_t x, uint16_t y, Color col)
{
    if (x >= 0 && y >= 0 && x < ge->winWidth && y < ge->winHeight)
        ge->pixels[x + y * ge->winWidth] = col;
}

void drawRect(GameEngine *ge, uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, Color col)
{
    for (int iy = 0; iy < dy; iy++)
        for (int ix = 0; ix < dx; ix++)
            draw(ge, x + ix, y + iy, col);
}

    //### Color ###
Color color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    Color col;
    col.r = r;
    col.g = g;
    col.b = b;
    col.a = a;
    return col;
}

Color colorD(uint32_t d)
{
    Color col;
    col.d = d;
    return col;
}

Color colorF(float r, float g, float b, float a)
{
    Color col;
    col.r = r * 255;
    col.g = g * 255;
    col.b = b * 255;
    col.a = a * 255;
    return col;
}



