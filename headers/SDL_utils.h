#ifndef SDL_UTILS
#define SDL_UTILS

#include <SDL2/SDL.h>
#include <stdbool.h>

bool init();
void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius);
void drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius);
void closeApp();

extern SDL_Window* window;
extern SDL_Renderer* renderer;

#endif