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
    newBall->angularVelocity = ((double)rand() / RAND_MAX) * 2.0 - 1;
    newBall->orientation = ((double)rand() / RAND_MAX) * 2.0 * M_PI;
    // genBallValues(newBall, borderBall, balls);

    printf("\n");
}

void genBallValues(Ball* ball, Ball* borderBall, BallArray* balls) {
    double centerX = SCREEN_WIDTH / 2.0;
    double centerY = SCREEN_HEIGHT / 2.0;

    // Generate random point inside the circle
    double angle = ((double)rand() / RAND_MAX) * 2.0 * M_PI;

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

    ball->angularVelocity = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
    ball->orientation = ((double)rand() / RAND_MAX) * 2.0 * M_PI;

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
            Vector newVel1, newVel2;
            calculateVelocities(ball->radius, ball->velocity, balls[j].radius, balls[j].velocity, &newVel1, &newVel2, ball->position, balls[j].position);

            // Stay outside of each other
            Vector collisionNormal = normalize(subtract(ball->position, balls[j].position));
            double overlap = ball->radius + balls[j].radius - magnitude(subtract(ball->position, balls[j].position));

            // Apply only a fraction of the overlap correction to avoid excessive impulses
            ball->position = add(ball->position, multiply(collisionNormal, overlap / 3.0)); 
            balls[j].position = subtract(balls[j].position, multiply(collisionNormal, overlap / 3.0));

            // Update linear velocities
            ball->velocity = multiply(newVel1, VELOCITY_MODIFIER);
            balls[j].velocity = multiply(newVel2, VELOCITY_MODIFIER);

            // Calculate relative velocity at the point of contact
            Vector r1 = multiply(collisionNormal, ball->radius);
            Vector r2 = multiply(collisionNormal, -balls[j].radius);
            Vector relativeVelocity = subtract(
                add(ball->velocity, (Vector){-ball->angularVelocity * r1.y, ball->angularVelocity * r1.x}),
                add(balls[j].velocity, (Vector){-balls[j].angularVelocity * r2.y, balls[j].angularVelocity * r2.x})
            );

            // Calculate tangential component of relative velocity
            Vector tangent = (Vector){-collisionNormal.y, collisionNormal.x};
            double tangentVelocity = dotProduct(relativeVelocity, tangent);

            // Apply friction to simulate rolling
            double frictionCoefficient = 0.05; // Adjust as needed
            double frictionImpulse = tangentVelocity * frictionCoefficient;

            ball->angularVelocity -= frictionImpulse / ball->radius;
            balls[j].angularVelocity += frictionImpulse / balls[j].radius;

            applyRollingPhysics(ball, &balls[j]);
        }
    }
}

void applyRollingPhysics(Ball* ball, Ball* otherBall) {
    if (magnitude(ball->velocity) < VELOCITY_THRESHOLD) {
        Vector distanceVec = subtract(otherBall->position, ball->position);
        Vector tangent = {-distanceVec.y, distanceVec.x};  // Perpendicular to the distance vector
        tangent = normalize(tangent);

        // Use angular velocity to influence rolling
        double rollingFriction = 0.01; // This value can be adjusted
        ball->velocity = multiply(tangent, ball->angularVelocity * rollingFriction);
    }
}


void updateBalls(Ball* ball, BallArray* ballsArray, Ball* borderBall, double deltaTime) {
    if (round(ball->position.y) == 330 && round(ball->velocity.x * 10) / 10 == 0) {
        ball->velocity.x = 0;
        ball->velocity.y = 0;
    } else {
        ball->velocity.y += GRAVITY * deltaTime;
    }

    // Collision thingin
    double distance = borderCollision(ball, borderBall);
    
    // Handle ball[i] going outside border
    handleOutOfBounds(ball, borderBall);

    handleBallCollision(ball, ballsArray->balls, ballsArray->size + 1, borderBall);
    // printf("ball[0] velocity: %f, %f\n", balls[0].velocity.x, balls[0].velocity.y);
    // printf("ball[0] magnitude of velocity: %f\n", magnitude(balls[0].velocity));



    // Update ball[i] position
    ball->position = add(ball->position, multiply(ball->velocity, deltaTime));

    // Update orientation based on angular velocity
    ball->orientation += ball->angularVelocity * deltaTime;

    // Ensure the orientation stays within 0 to 2*PI
    if (ball->orientation >= 2.0 * M_PI) {
        ball->orientation -= 2.0 * M_PI;
    } else if (ball->orientation < 0) {
        ball->orientation += 2.0 * M_PI;
    }

    // printf("Ball %d: posX: %f, posY: %f, velX: %f, velY: %f\n", i, ballsArray->balls[i].position.x, ballsArray->balls[i].position.y, ballsArray->balls[i].velocity.x, ballsArray->balls[i].velocity.y);

    // printf("RUNNING MAIN FOR LOOP ITERATION %d\n", i);


    ////////////////////////// IF MAGNITUDE OF VELOCITY IS LESS THAN 0.1 THEN CANCEL GRAVITY, DO THAT BY ADDING GRAVITY PROPERTY TO BALL AND ONLY ADDING GRAVITY IF THAT PROPERTY IS TRUE

}