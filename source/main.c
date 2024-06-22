#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "vector.h"
#include "sdl_utils.h"
#include "ball.h"
#include "common.h"

/* 
###### BUG LIST
 
1. Ball dissapears after collision - only seen once
2. Generated balls sometimes get stuck in border
3. Values has to be tweaked
4. Smaller ball should have less force
5. after a while the balls get so much momentum they wont stop
*/


int main(int argc, char* argv[]) {
    bool exit = false;
    SDL_Event e;
    Uint32 lastTime = SDL_GetTicks();

    if (!init()) {
        printf("Failure to initialize\n");
    } else {
        Ball borderBall = {{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, {0, 0}, 400};

        Ball* balls = (Ball*)malloc(sizeof(Ball) * numBalls);
        if (balls == NULL) {
            printf("Failure to allocate memory\n");
            return 1;
        }

        srand(time(NULL));
        generateBalls(numBalls, balls, &borderBall);

        while (!exit) {
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
                    int mouseX, mouseY;
                    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
                    balls[0].position.x = mouseX;
                    balls[0].position.y = mouseY;
                    balls[0].velocity.y = 2;
                    balls[0].velocity.x = 1;
                }
            }

            // Fine test of using delta time, didnt work :(
            // Uint32 currentTime = SDL_GetTicks();
            // double deltaTime = currentTime - lastTime / 1000;
            // lastTime = currentTime;

            /* BALL 1 */

            // STOP BALL FROM BOUNCING IF IT IS STUCK IN MIDDLE WITH NO X VELOCITY
            for (int i = 0; i < numBalls; i++) {   
                if (TIME_STEP == 0) {
                    SDL_Delay(100000);
                }

                if (round(balls[i].position.y) == 330 && round(balls[i].velocity.x * 10) / 10 == 0) {
                    balls[i].velocity.x = 0;
                    balls[i].velocity.y = 0;
                } else {
                    balls[i].velocity.y += GRAVITY * TIME_STEP;
                }

                // Collision thingin
                double distance = borderCollision(&balls[i], &borderBall);
                
                // Handle ball[i] going outside border
                handleOutOfBounds(&balls[i], &borderBall);

                // Update ball[i] position
                balls[i].position = add(balls[i].position, multiply(balls[i].velocity, TIME_STEP));

                // printf("Ball %d: posX: %f, posY: %f, velX: %f, velY: %f\n", i, balls[i].position.x, balls[i].position.y, balls[i].velocity.x, balls[i].velocity.y);

                // printf("RUNNING MAIN FOR LOOP ITERATION %d\n", i);

                handleBallCollision(&balls[i], balls, numBalls, &borderBall);
                printf("ball[0] velocity: %f, %f\n", balls[0].velocity.x, balls[0].velocity.y);
                printf("ball[0] magnitude of velocity: %f\n", magnitude(balls[0].velocity));

                ////////////////////////// IF MAGNITUDE OF VELOCITY IS LESS THAN 0.1 THEN CANCEL GRAVITY, DO THAT BY ADDING GRAVITY PROPERTY TO BALL AND ONLY ADDING GRAVITY IF THAT PROPERTY IS TRUE

                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
                drawCircle(renderer, balls[i].position.x, balls[i].position.y, balls[i].radius);
            }

            // Update screen
            SDL_RenderPresent(renderer);

            // Delay for approx. 30 fps
            SDL_Delay(16) ; // 32 ms
        }
        free(balls);

        closeApp();
    }

    return 0;
}