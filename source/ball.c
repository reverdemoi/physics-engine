#include "ball.h"
#include "vector.h"
#include "common.h"
#include "SDL_utils.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

const int VELOCITY_THRESHOLD = 1;

void initBallArray(BallArray *balls) {
    balls->balls = (Ball*)malloc(sizeof(Ball));
    if (balls->balls == NULL) {
        perror("Initial malloc failed");
        closeApp();
    }
    balls->size = 0;
    balls->capacity = 1;
}

void freeBallArray(BallArray *balls) {
    free(balls->balls);
    balls->size = 0;
    balls->capacity = 0;
}

void newBall(Ball* borderBall, BallArray* balls) {
    *borderBall = (Ball){{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, {0, 0}, 250};
    
    if (balls->size >= balls->capacity) {
        balls->capacity *= 2;
        // printf("Resized array to %d\n", balls->capacity);
        Ball* newBalls = (Ball*)realloc(balls->balls, balls->capacity * sizeof(Ball));
        if (newBalls == NULL) {
            printf("realloc failure\n");
            freeBallArray(balls);
            free(balls);
            closeApp();
            return;
        }

        balls->balls = newBalls;
    }
    Ball *newBall = &balls->balls[balls->size];
    balls->size++;

    genBallValues(newBall, borderBall, balls);
}

void genBallValues(Ball* ball, Ball* borderBall, BallArray* balls) {
    double centerX = SCREEN_WIDTH / 2.0;
    double centerY = SCREEN_HEIGHT / 2.0;

    ball->position = (Vector){centerX, centerY};
    ball->radius = 10;
    ball->gravity = true;
    ball->ballNumber = balls->size;

    int mouseX, mouseY;
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
    Vector mouseVec = {mouseX, mouseY};

    // Ball goes towards mouse
    Vector normal = normalize(subtract(mouseVec, borderBall->position));
    ball->velocity = multiply(normal, 100.0);
}

double borderCollision(Ball* ball, Ball* borderBall) {
    Vector distanceVec = subtract(ball->position, borderBall->position);
    double distance = magnitude(distanceVec);

    if (distance >= (borderBall->radius - ball->radius)) {
        Vector normal = normalize(distanceVec);
        double dot = dotProduct(ball->velocity, normal);
        Vector scaledNormal = multiply(normal, 2 * dot);
        ball->velocity = subtract(ball->velocity, scaledNormal);

        ball->velocity = multiply(ball->velocity, VELOCITY_MODIFIER);
    }

    return distance;
}

void handleOutOfBounds(Ball* ball, Ball* borderBall) {
    Vector distanceVec = subtract(ball->position, borderBall->position);
    double distance = magnitude(distanceVec);            

    if (distance >= (borderBall->radius - ball->radius)) {
        Vector inverseNormal = multiply(normalize(distanceVec), -1);
        
        // Calculate the angle between the normal and the standard vector
        Vector standard = {1, 0};
        double angle = acos((dotProduct(inverseNormal, standard)) / magnitude(inverseNormal) * magnitude(standard));

        // Calculate position of the point where ball would have surpassed the border
        Ball Point;
        Point.position.x = (borderBall->position.x + (int)round((borderBall->radius - ball->radius) * cos(angle))) * -1 + SCREEN_WIDTH;
        Point.position.y = borderBall->position.y + (int)round((borderBall->radius - ball->radius) * sin(angle));

        // If it's above the center of the screen, adjust the y value to match ball
        if (ball->position.y < SCREEN_HEIGHT / 2) {
            Point.position.y = (borderBall->position.y + (int)round((borderBall->radius - ball->radius) * sin(angle))) * -1 + SCREEN_HEIGHT;
        }

        ball->position = Point.position;
    } 
}

bool collisionCheck(Ball* ball1, Ball* ball2) {
    Vector distanceVec = subtract(ball1->position, ball2->position);
    double distance = magnitude(distanceVec);

    if (distance <= (ball1->radius + ball2->radius)) {
        return true;
    } else {
        return false;
    }
}

void calculateVelocities(double mass1, Vector vel1, double mass2, Vector vel2, Vector *newVel1, Vector *newVel2, Vector r1, Vector r2) {
    /* CHARREN KOKADE HÄR */
    Vector r = subtract(r2, r1);
    Vector vel = subtract(vel2, vel1);

    Vector normal = normalize(r);

    double v_rel_n = dotProduct(vel, normal);

    if (v_rel_n > 0) {
        *newVel1 = vel1;
        *newVel2 = vel2;
        return;
    }

    *newVel1 = add(vel1, multiply(normal, 2 * mass2 / (mass1 + mass2) * v_rel_n));
    *newVel2 = subtract(vel2, multiply(normal, 2 * mass1 / (mass1 + mass2) * v_rel_n));
}
void handleBallCollision(Ball* ball, Ball* balls, int numBalls, Ball* borderBall) {
    for (int j = 0; j < numBalls; j++) {
        if (ball == &balls[j]) {
            continue; // Skip the same ball
        }

        if (collisionCheck(ball, &balls[j])) {
            Vector newVel1, newVel2;
            calculateVelocities(ball->radius, ball->velocity, balls[j].radius, balls[j].velocity, &newVel1, &newVel2, ball->position, balls[j].position);

            // Stay outside of each other
            Vector collisionNormal = normalize(subtract(ball->position, balls[j].position));
            double overlap = ball->radius + balls[j].radius - magnitude(subtract(ball->position, balls[j].position));
            ball->position = add(ball->position, multiply(collisionNormal, overlap / 2));  
            balls[j].position = subtract(balls[j].position, multiply(collisionNormal, overlap / 2));

            ball->velocity = multiply(newVel1, VELOCITY_MODIFIER);
            balls[j].velocity = multiply(newVel2, VELOCITY_MODIFIER);
        }
    }
}

void updateBalls(Ball* ball, BallArray* ballsArray, Ball* borderBall, double deltaTime) {
    ball->velocity.y += pow(GRAVITY, 2) * deltaTime;

    double distance = borderCollision(ball, borderBall);    
    handleOutOfBounds(ball, borderBall);
    handleBallCollision(ball, ballsArray->balls, ballsArray->size + 1, borderBall);

    ball->position = add(ball->position, multiply(ball->velocity, deltaTime));
}

// I THINK THE EXCESSIVE JITTERING IS BECAUSE THE BALLS ACCIDENTLY GO HALFWAY THROUGH EACH OTHER WHICH WILL RESULT IN THE OVERLAP VELOCITY CHANGE TO THROW THEM EACH AT THE OTHER ONES DIRECTION - CAUSING MASS EXCESSIVE JITTERING