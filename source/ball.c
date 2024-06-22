#include "ball.h"
#include "vector.h"
#include "common.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

void generateBalls(int numBalls, Ball* balls, Ball* borderBall) {
    double centerX = SCREEN_WIDTH / 2.0;
    double centerY = SCREEN_HEIGHT / 2.0;

    for (int i = 0; i < numBalls; i++) {
        // Generate random point inside the circle
        double angle = ((double)rand() / RAND_MAX) * 2.0 * M_PI;
        double radius = sqrt((double)rand() / RAND_MAX) * borderBall->radius; // sqrt for uniform distribution

        balls[i].position.x = centerX + radius * cos(angle);
        balls[i].position.y = centerY + radius * sin(angle);

        // Random velocity and radius
        balls[i].velocity.x = ((double)rand() / RAND_MAX) * 10.0 + 1.0;
        balls[i].velocity.y = ((double)rand() / RAND_MAX) * 10.0 + 1.0;
        balls[i].radius = ((double)rand() / RAND_MAX) * 20.0 + 10.0;
        balls[i].gravity = true;
    }

    // print all values od balls
    for (int i = 0; i < numBalls; i++) {
        printf("Ball %d: posX: %f, posY: %f, velX: %f, velY: %f, radius: %f\n", i, balls[i].position.x, balls[i].position.y, balls[i].velocity.x, balls[i].velocity.y, balls[i].radius);
    }
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
            printf("BALL COLLISION\n");

            Vector newVel1, newVel2;
            calculateVelocities(ball->radius, ball->velocity, balls[j].radius, balls[j].velocity, &newVel1, &newVel2, ball->position, balls[j].position);

            // check if stuck inside
            Vector normalVel1 = normalize(newVel1);
            Vector normalVel2 = normalize(newVel2);

            Vector newPos1 = add(ball->position, multiply(newVel1, TIME_STEP)); 
            Vector newPos2 = add(balls[j].position, multiply(newVel2, TIME_STEP));

            Ball newBall1 = {newPos1, newVel1, ball->radius};
            Ball newBall2 = {newPos2, newVel2, balls[j].radius};

            if (collisionCheck(&newBall1, &newBall2)) {
                printf("BALL STUCK\n");
                // printf("BALL1: magnitude of velocity: %f\n", magnitude(newVel1));
                // printf("BALL1: normal of velocity, x: %f, y: %f\n", normalVel1.x, normalVel1.y);
                // printf("BALL2: magnitude of velocity: %f\n", magnitude(newVel2));
                // printf("BALL2: normal of velocity, x: %f, y: %f\n", normalVel2.x, normalVel2.y);
                // ball->velocity = multiply(ball->velocity, -1);
                // balls[j].velocity = multiply(balls[j].velocity, -1);
                return;
            }

            // Check if still inside
            // distanceVec = subtract(ball->position, balls[j].position);
            // distance = magnitude(distanceVec);

            //     if (distance <= (ball->radius + balls[j].radius)) {
            //         double angle = ((double)rand() / RAND_MAX) * 2.0 * M_PI;
            //         double radius = sqrt((double)rand() / RAND_MAX) * borderBall->radius; // sqrt for uniform distribution

            //         ball->position.x = borderBall->position.x + radius * cos(angle);
            //         ball->position.y = borderBall->position.y + radius * sin(angle);
            //   }


            ball->velocity = newVel1;
            balls[j].velocity = newVel2;

            ball->velocity = multiply(ball->velocity, VELOCITY_MODIFIER);
            balls[j].velocity = multiply(balls[j].velocity, VELOCITY_MODIFIER);
        }
    }
}
