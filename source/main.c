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

    2. Medium Priority

        [x] - Smaller ball should have less force

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

void initBallArray(BallArray *balls, double capacity) {
    balls->balls = (Ball*)malloc(capacity * sizeof(Ball));
    if (balls->balls == NULL) {
        perror("Initial malloc failed");
        exit(EXIT_FAILURE); // Exit the program if the function fails
    }
    balls->size = 0;
    balls->capacity = capacity;
}

void freeBallArray(BallArray *balls) {
    free(balls->balls);
    balls->size = 0;
    balls->capacity = 0;
}

void newBall(Ball* borderBall, BallArray* balls) {
    *borderBall = (Ball){{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, {0, 0}, 250};
    
    if (balls->size >= balls->capacity) {
        balls->capacity++;
        printf("Resized array to %d\n", balls->capacity);
        Ball* newBalls = (Ball*)realloc(balls->balls, balls->capacity * sizeof(Ball));
        if (newBalls == NULL) {
            printf("realloc failure\n");
            freeBallArray(balls);
            free(balls);
            closeApp();
            exit(EXIT_FAILURE);
            return;
        }

        balls->balls = newBalls;
    }

    printf("New ball craeted at position: %i\n", balls->size);
    Ball *newBall = &balls->balls[balls->size];
    printf("ball created\n");
    balls->size++;

    // printf("newBall position: %f, %f\n", newBall->position.x, newBall->position.y);
    double centerX = SCREEN_WIDTH / 2.0;
    double centerY = SCREEN_HEIGHT / 2.0;

    // Generate random point inside the circle
    double angle = ((double)rand() / RAND_MAX) * 2.0 * M_PI;
    double radius = sqrt((double)rand() / RAND_MAX) * borderBall->radius; // sqrt for uniform distribution

    newBall->position.x = centerX + radius * cos(angle);
    newBall->position.y = centerY + radius * sin(angle);

    // Random velocity and radius
    newBall->velocity.x = ((double)rand() / RAND_MAX) * 10.0 + 1.0;
    newBall->velocity.y = ((double)rand() / RAND_MAX) * 10.0 + 1.0;
    newBall->radius = ((double)rand() / RAND_MAX) * 20.0 + 10.0;
    newBall->gravity = true;
    // genBallValues(newBall, borderBall);
    printf("ball values generated");

    printf("\n");
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

        initBallArray(ballsArray, 10);

        int64_t lastTick = GetTicks() / 1000000;

        while (!exit) {
            frameStart = SDL_GetTicks();

            int64_t ticks = GetTicks() / 1000000;
            // printf("Ticks: %lld\n", ticks / 1000000);

            if (ticks == lastTick + 5) {
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