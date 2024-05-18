#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

bool init() {
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failure to initialize, SDL_Error: %s\n", SDL_GetError());
        success = false;
    } else {
        window = SDL_CreateWindow( "physics", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

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

void closeApp() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
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

void boarderCollision(int* ballX, int* ballY, int* velX, int* velY, double ballRadius, int* boarderCenterX, int* boarderCenterY, double boarderRadius) {
    double distance = round(sqrt(pow((*boarderCenterX - *ballX), 2) + pow((*boarderCenterY - *ballY), 2)));

    // printf("Distance: %f\n", distance);
    if (distance + ballRadius >= boarderRadius) {
        *velX = -(*velX) * 0.66;
        *velY = -(*velY) * 0.66;

        printf("velY = %d\n", *velY);
    }
}

int main (int argc, char* argv[] ) {
    bool exit = false;

    SDL_Event e;

    // MAKE A STRUCT FOR A BALL
    int circleX = SCREEN_WIDTH / 2;
    int circleY = SCREEN_HEIGHT / 2;
    double radius = 10;
    int velX = 1;
    int velY = 1;
    int velMod = 1;

    int boarderCircleX = SCREEN_WIDTH / 2;
    int boarderCircleY = SCREEN_HEIGHT / 2;
    double boarderCircleRadius = 100;

    if (!init()) {
        printf("Failure to initialize");

    } else {
        while (!exit) {
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(renderer);
            // printf("%s\n", hasCollided);

            // Input events
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    exit = true;
                }

                // if (e.type == SDL_KEYDOWN) {
                //     switch(e.key.keysym.sym) {
                //         case SDLK_UP:
                //             circleY -= velY;
                //             velY += velMod;
                //             break;
                        
                //         case SDLK_DOWN:
                //             circleY += velY;
                //             velY += velMod;
                //             break;

                //         case SDLK_LEFT:
                //             circleX -= velX;
                //             velX += velMod;
                //             break;

                //         case SDLK_RIGHT:
                //             circleX += velX;
                //             velX += velMod;
                //             break;
                //     }
                // } else {
                //     velX = velMod; 
                //     velY = velMod;
                // }
            }

            circleY += velY;
            velY += velMod; 

            boarderCollision(&circleX, &circleY, &velX, &velY, radius, &boarderCircleX, &boarderCircleY, boarderCircleRadius);




            // Boarder collision:
            // if (circleX <= radius) {
            //     circleX = radius;
            // }
            // if (circleX >= SCREEN_WIDTH - radius) {
            //     circleX = SCREEN_WIDTH - radius;
            // }
            // if (circleY <= radius) {
            //     circleY = radius;
            // }
            // if (circleY >= SCREEN_HEIGHT - radius) {
            //     circleY = SCREEN_HEIGHT - radius;
            // }


            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

            // Make ball follow mouse position
            // int mouseX, mouseY;
            // Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
            // circleX = mouseX;
            // circleY = mouseY;

            // Draw boarder
            drawCircle(renderer, boarderCircleX, boarderCircleY, boarderCircleRadius);

            // Draw ball
            drawCircle(renderer, circleX, circleY, radius);

            SDL_RenderPresent(renderer);
            
            SDL_Delay(16);
        }
    }
}
