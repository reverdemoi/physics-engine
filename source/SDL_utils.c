#include "SDL_utils.h"
#include "common.h"
#include <stdio.h>
#include <windows.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

bool init() {
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failure to initialize, SDL_Error: %s\n", SDL_GetError());
        success = false;
    } else {
        window = SDL_CreateWindow("Physics Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if (window == NULL) {
            printf("Failure to create window, SDL_Error: %s\n", SDL_GetError());
        } else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

            if (renderer == NULL) {
                printf("Failure to create renderer, SDL_Error: %s\n", SDL_GetError());
            } else {
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }

    return success;
}

void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y) {
        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }

        if (err > 0) {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

void drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
        int x = radius;
    int y = 0;
    int radiusError = 1 - x;

    while (x >= y) {
        // Draw horizontal lines from the center to the circle's edge
        SDL_RenderDrawLine(renderer, centerX - x, centerY + y, centerX + x, centerY + y);
        SDL_RenderDrawLine(renderer, centerX - y, centerY + x, centerX + y, centerY + x);
        SDL_RenderDrawLine(renderer, centerX - x, centerY - y, centerX + x, centerY - y);
        SDL_RenderDrawLine(renderer, centerX - y, centerY - x, centerX + y, centerY - x);

        y++;
        if (radiusError < 0) {
            radiusError += 2 * y + 1;
        } else {
            x--;
            radiusError += 2 * (y - x + 1);
        }
    }
}

void closeApp() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    exit(EXIT_FAILURE);
}
