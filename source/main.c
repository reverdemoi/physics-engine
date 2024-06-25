#include <SDL2/SDL.h>
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "vector.h"
#include "sdl_utils.h"
#include "ball.h"
#include "common.h"

/* 

###### BUG LIST / TODO LIST ######

    1. High Priority

        [] - Balls start moving like popping popcorn after a while - probably due to the ball collision handling
        [] - Generated balls sometimes get stuck in border
        [x] - Make new balls have velocity towards mouse position

    2. Medium Priority

        [x] - Smaller ball should have less force
        [x] - Separate generation of values for new balls into separate function

    3. Low Priority

        [] - Values has to be tweaked
        [] - Ball dissapears after collision - only seen once

*/

static inline int64_t GetTicks() {
    LARGE_INTEGER ticks;
    if (!QueryPerformanceCounter(&ticks)) {
        perror("QueryPerformanceCounter failed");
        exit(EXIT_FAILURE); // Exit the program if the function fails
    }
    return ticks.QuadPart;
}

int main(int argc, char* argv[]) {
    bool exit = false;
    SDL_Event e;
    Uint32 lastTime = SDL_GetTicks();
    Uint32 frameStart;
    Uint32 frameTime;
    int frameCount = 0;
    float fps = 0.0f;

    if (!init()) {
        printf("Failure to initialize\n");
    } else {
        Ball borderBall;
        BallArray *ballsArray = (BallArray * )malloc(sizeof(BallArray));

        initBallArray(ballsArray);

        int64_t lastTick = GetTicks() / 1000000;
    
        Uint32 previousTicks = SDL_GetTicks();
        Uint32 currentTicks;
        double deltaTime;

        while (!exit) {
            frameStart = SDL_GetTicks();

            int64_t ticks = GetTicks() / 1000000;
            // printf("Ticks: %lld\n", ticks / 1000000);

            // Calculate deltaTime
            currentTicks = SDL_GetTicks();
            deltaTime = (currentTicks - previousTicks) / 1000.0;
            previousTicks = currentTicks;

            printf("Delta time: %f\n", deltaTime);

            // if (ticks == lastTick + 4) {
            //     // printf("BALL ADDED");
            //     lastTick = ticks;

            //     newBall(&borderBall, ballsArray);
            // }

            // make background black
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
            SDL_RenderClear(renderer);

            // make a white ball in the middle, filled in
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            drawFilledCircle(renderer, borderBall.position.x, borderBall.position.y, borderBall.radius - 2);

            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    exit = true;
                }

                if (e.type == SDL_MOUSEBUTTONDOWN) {
                    // DEBUG - teleport ball to mouse position
                    // int mouseX, mouseY;
                    // Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
                    // balls[0].position.x = mouseX;
                    // balls[0].position.y = mouseY;
                    // balls[0].velocity.y = 2;
                    // balls[0].velocity.x = 1;
                    // balls = initBalls(&borderBall, numBalls);
                    newBall(&borderBall, ballsArray);
                }
            }

            // numBalls++;

            // Fine test of using delta time, didnt work :(
            // Uint32 currentTime = SDL_GetTicks();
            // double deltaTime = currentTime - lastTime / 1000;
            // lastTime = currentTime;


            // STOP BALL FROM BOUNCING IF IT IS STUCK IN MIDDLE WITH NO X VELOCITY
            for (int i = 0; i < ballsArray->size; i++) {   
                updateBalls(&ballsArray->balls[i], ballsArray, &borderBall, deltaTime * 5);

                if (ballsArray->balls[i].angularVelocity > 0) {
                    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0xFF, 0xFF); // cyan
                }
                if (ballsArray->balls[i].angularVelocity < 0) {
                    SDL_SetRenderDrawColor(renderer, 0x4B, 0x00, 0x82, 0xFF);
                }
                if (ballsArray->balls[i].angularVelocity == 0) {
                    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
                }
                drawFilledCircle(renderer, ballsArray->balls[i].position.x, ballsArray->balls[i].position.y, ballsArray->balls[i].radius);
                // drawCircle(renderer, ballsArray->balls[i].position.x, ballsArray->balls[i].position.y, ballsArray->balls[i].radius);
            }


            // Update screen
            SDL_RenderPresent(renderer);

            frameTime = SDL_GetTicks() - frameStart;
            frameCount++;
            if (frameTime > 0) {
                fps = 1000.0f / frameTime;
            }

            if (frameCount % 60 == 0) {
                // printf("FPS: %f\n", fps);
            }

            // Delay for approx. 30 fps
            SDL_Delay(16) ; // 32 ms
        }
        free(ballsArray);

        closeApp();
    }

    return 0;
}