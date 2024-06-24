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
        [] - Make new balls have velocity towards mouse position

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

        while (!exit) {
            frameStart = SDL_GetTicks();

            int64_t ticks = GetTicks() / 1000000;
            // printf("Ticks: %lld\n", ticks / 1000000);

            if (ticks == lastTick + 1) {
                // printf("BALL ADDED");
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

                if (e.type == SDL_MOUSEBUTTONDOWN) {
                    // DEBUG - teleport ball to mouse position
                    // int mouseX, mouseY;
                    // Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
                    // balls[0].position.x = mouseX;
                    // balls[0].position.y = mouseY;
                    // balls[0].velocity.y = 2;
                    // balls[0].velocity.x = 1;
                    // balls = initBalls(&borderBall, numBalls);
                }
            }

            // numBalls++;

            // Fine test of using delta time, didnt work :(
            // Uint32 currentTime = SDL_GetTicks();
            // double deltaTime = currentTime - lastTime / 1000;
            // lastTime = currentTime;


            // STOP BALL FROM BOUNCING IF IT IS STUCK IN MIDDLE WITH NO X VELOCITY
            for (int i = 0; i < ballsArray->size; i++) {   
                if (TIME_STEP == 0) {
                    SDL_Delay(100000);
                }

                if (round(ballsArray->balls[i].position.y) == 330 && round(ballsArray->balls[i].velocity.x * 10) / 10 == 0) {
                    ballsArray->balls[i].velocity.x = 0;
                    ballsArray->balls[i].velocity.y = 0;
                } else {
                    ballsArray->balls[i].velocity.y += GRAVITY * TIME_STEP;
                }

                // Collision thingin
                double distance = borderCollision(&ballsArray->balls[i], &borderBall);
                
                // Handle ball[i] going outside border
                handleOutOfBounds(&ballsArray->balls[i], &borderBall);

                handleBallCollision(&ballsArray->balls[i], ballsArray->balls, ballsArray->size + 1, &borderBall);
                // printf("ball[0] velocity: %f, %f\n", balls[0].velocity.x, balls[0].velocity.y);
                // printf("ball[0] magnitude of velocity: %f\n", magnitude(balls[0].velocity));

                // Update ball[i] position
                ballsArray->balls[i].position = add(ballsArray->balls[i].position, multiply(ballsArray->balls[i].velocity, TIME_STEP));

                // printf("Ball %d: posX: %f, posY: %f, velX: %f, velY: %f\n", i, ballsArray->balls[i].position.x, ballsArray->balls[i].position.y, ballsArray->balls[i].velocity.x, ballsArray->balls[i].velocity.y);

                // printf("RUNNING MAIN FOR LOOP ITERATION %d\n", i);


                ////////////////////////// IF MAGNITUDE OF VELOCITY IS LESS THAN 0.1 THEN CANCEL GRAVITY, DO THAT BY ADDING GRAVITY PROPERTY TO BALL AND ONLY ADDING GRAVITY IF THAT PROPERTY IS TRUE

                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
                drawCircle(renderer, ballsArray->balls[i].position.x, ballsArray->balls[i].position.y, ballsArray->balls[i].radius);
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