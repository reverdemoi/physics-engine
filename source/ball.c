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
        exit(EXIT_FAILURE); // Exit the program if the function fails
    }
    balls->size = 0;
    balls->capacity = 0;
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

    printf("New ball created at position: %i\n", balls->size);
    Ball *newBall = &balls->balls[balls->size];
    printf("ball created\n");
    balls->size++;

    newBall->position = (Vector){SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0};
    newBall->velocity = (Vector){1, 1};
    newBall->radius = 25;
    newBall->gravity = true;
    // genBallValues(newBall, borderBall);

    printf("\n");
}

void genBallValues(Ball* ball, Ball* borderBall, BallArray* balls) {
    double centerX = SCREEN_WIDTH / 2.0;
    double centerY = SCREEN_HEIGHT / 2.0;

    // Generate random point inside the circle
    double angle = ((double)rand() / RAND_MAX) * 2.0 * M_PI;
    double radius = sqrt((double)rand() / RAND_MAX) * borderBall->radius; // sqrt for uniform distribution

    // ball->position.x = centerX + radius * cos(angle);
    // ball->position.y = centerY + radius * sin(angle);
    ball->position = (Vector){centerX, centerY};

    // Random velocity and radius
    // ball->velocity.x = ((double)rand() / RAND_MAX) * 10.0 + 1.0;
    // ball->velocity.y = ((double)rand() / RAND_MAX) * 10.0 + 1.0;
    // ball->radius = ((double)rand() / RAND_MAX) * 10.0 + 5.0;
    ball->radius = 15;
    ball->gravity = true;
    ball->ballNumber = balls->size;

    int mouseX, mouseY;
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
    Vector mouseVec = {mouseX, mouseY};

    // Calculate the angle between the mouse and the center of the circle
    Vector normal = normalize(subtract(mouseVec, borderBall->position));
    ball->velocity = multiply(normal, 50.0);
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

        // DEBUG
        // printf("velX: %f, velY: %f, posX: %f, posY: %f\n", ball->velocity.x, ball->velocity.y, ball->position.x, ball->position.y);
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

    // *newVel1 = multiply(vel1, -1);
    // *newVel2 = multiply(vel2, -1);
}

void handleBallCollision(Ball* ball, Ball* balls, int numBalls, Ball* borderBall) {
    for (int j = 0; j < numBalls; j++) {
        if (ball == &balls[j]) {
            continue; // Skip the same ball
        }

        if (collisionCheck(ball, &balls[j])) {
            // printf("BALL COLLISION\n");

            Vector newVel1, newVel2;
            calculateVelocities(ball->radius, ball->velocity, balls[j].radius, balls[j].velocity, &newVel1, &newVel2, ball->position, balls[j].position);

            /* STAY OUTSIDE OF EACH OTHER */
            Vector collisionNormal = normalize(subtract(ball->position, balls[j].position));
            double overlap = ball->radius + balls[j].radius - magnitude(subtract(ball->position, balls[j].position));

            ball->position = add(ball->position, multiply(collisionNormal, overlap / 2.0)); 
            balls[j].position = subtract(balls[j].position, multiply(collisionNormal, overlap / 2.0));

            /* UPDATE VELOCITIES */
            ball->velocity = newVel1;
            balls[j].velocity = newVel2;

            ball->velocity = multiply(ball->velocity, VELOCITY_MODIFIER);
            balls[j].velocity = multiply(balls[j].velocity, VELOCITY_MODIFIER);

            applyRollingPhysics(ball, &balls[j]);
        }
    }
}

void applyRollingPhysics(Ball* ball, Ball* otherBall) {
    // printf("ball number: %d, has mag. of vel.:%f\n", ball->ballNumber, magnitude(ball->velocity));
    if (magnitude(ball->velocity) < VELOCITY_THRESHOLD) {
        Vector distanceVec = subtract(otherBall->position, ball->position);
        Vector tangent = {-distanceVec.y, distanceVec.x};  // Perpendicular to the distance vector
        tangent = normalize(tangent);
        ball->velocity = multiply(tangent, 0.1);  // Apply a small tangential force to simulate rolling
    }
}