#ifndef BALL_H
#define BALL_H

#include <stdbool.h>
#include "vector.h"

typedef struct {
    Vector position;
    Vector velocity;
    double radius;
    bool gravity;
    double ballNumber;
} Ball;

typedef struct {
    Ball *balls;
    int size;
    int capacity;
} BallArray;

void initBallArray(BallArray *balls);
void freeBallArray(BallArray *balls);
void newBall(Ball* borderBall, BallArray* balls);
void genBallValues(Ball* ball, Ball* borderBall, BallArray* balls);
double borderCollision(Ball* ball, Ball* borderBall);
void handleOutOfBounds(Ball* ball, Ball* borderBall);
bool collisionCheck(Ball* ball1, Ball* ball2);
void calculateVelocities(double mass1, Vector vel1, double mass2, Vector vel2, Vector *newVel1, Vector *newVel2, Vector r1, Vector r2);
void handleBallCollision(Ball* ball, Ball* balls, int numBalls, Ball* borderBall);
void applyRollingPhysics(Ball* ball, Ball* otherBall);
void updateBalls(Ball* ball, BallArray* ballsArray, Ball* borderBall, double deltaTime);

#endif