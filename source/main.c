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

        [] - Generated balls sometimes get stuck in border
            --> not necessarily fixed, now using a different method of spawning new balls in which makes this an irrelevant issue
        [x] - Balls start moving like popping popcorn after a while - probably due to the ball collision handling
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
        closeApp();
    }
    return ticks.QuadPart;
}

int main(int argc, char* argv[]) {
    if (!init()) {
        printf("Failure to initialize\n");
    } 

    bool exit = false;
    SDL_Event e;
    
    // FPS counter variables
    Uint32 lastTime = SDL_GetTicks();
    Uint32 frameStart;
    Uint32 frameTime;
    int frameCount = 0;
    float fps = 0.0f;

    Ball borderBall;
    BallArray *ballsArray = (BallArray * )malloc(sizeof(BallArray));

    initBallArray(ballsArray);
    
    // spawning new balls
    int64_t lastTick = GetTicks() / 1000000; 

    // Deltatime
    Uint32 previousTicks = SDL_GetTicks();
    Uint32 currentTicks;
    double deltaTime;

    while (!exit) {
        // Calculate deltaTime, FPS & spawning new balls
        int64_t ticks = GetTicks() / 1000000;
        currentTicks = SDL_GetTicks();
        deltaTime = (currentTicks - previousTicks) / 1000.0;
        previousTicks = currentTicks;

        if (ticks == lastTick + 2) {
            lastTick = ticks;
            newBall(&borderBall, ballsArray);
        }

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
        }

        for (int i = 0; i < ballsArray->size; i++) {   
            updateBalls(&ballsArray->balls[i], ballsArray, &borderBall, deltaTime * 5);

            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); 
            drawCircle(renderer, ballsArray->balls[i].position.x, ballsArray->balls[i].position.y, ballsArray->balls[i].radius);
        }

        // Update screen
        SDL_RenderPresent(renderer);

        // FPS Counter
        frameTime = SDL_GetTicks() - currentTicks; // currentTicks = frameStart
        frameCount++;
        if (frameTime > 0) {
            fps = 1000.0f / frameTime;
        }

        if (frameCount % 60 == 0) {
            // printf("FPS: %f\n", fps);
        }

        // Delay for approx. 60 fps
        // For some reason this is fucking important for the balls to move properly even when using deltaTime
        SDL_Delay(16); // 16 ms
    }
    free(ballsArray);

    closeApp();

    return 0;
}