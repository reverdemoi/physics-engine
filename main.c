#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
const double TIME_STEP = 0.1;
const double VELOCITY_MODIFIER = 0.8;
double GRAVITY = 9.8;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

typedef struct {
    double x;
    double y;
} Vector;

typedef struct {
    Vector position;
    Vector velocity;
    double radius;
} Ball;

double magnitude(Vector v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

Vector normalize(Vector v) {
    double mag = magnitude(v);
    Vector result = {v.x / mag, v.y / mag};
    return result;
}

Vector subtract(Vector a, Vector b) {
    Vector result = {a.x - b.x, a.y - b.y};
    return result;
}

double dotProduct(Vector a, Vector b) {
    return a.x * b.x + a.y * b.y;
}

Vector multiply(Vector v, double scalar) {
    Vector result = {v.x * scalar, v.y * scalar};
    return result;
}

Vector add(Vector a, Vector b) {
    Vector result = {a.x + b.x, a.y + b.y};
    return result;
}

bool init() {
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failure to initialize, SDL_Error: %s\n", SDL_GetError());
        success = false;
    } else {
        window = SDL_CreateWindow("Physics Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if (window == NULL) {
            printf("Failure to create window, SDL_Error: %s\n", SDL_GetError());
        } else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

            if (renderer == NULL) {
                printf("Failure to create renderer, SDL_Error: %s\n", SDL_GetError());
            } else {
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }

    return success;
}

void closeApp() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y) {
        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }

        if (err > 0) {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
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
    }

    // print all values od balls
    for (int i = 0; i < numBalls; i++) {
        printf("Ball %d: posX: %f, posY: %f, velX: %f, velY: %f, radius: %f\n", i, balls[i].position.x, balls[i].position.y, balls[i].velocity.x, balls[i].velocity.y, balls[i].radius);
    }
}

int main(int argc, char* argv[]) {
    bool exit = false;
    SDL_Event e;
    Uint32 lastTime = SDL_GetTicks();

    if (!init()) {
        printf("Failure to initialize\n");
    } else {
        int numBalls = 10;
        Ball borderBall = {{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, {0, 0}, 100};

        Ball* balls = (Ball*)malloc(sizeof(Ball) * numBalls);
        if (balls == NULL) {
            printf("Failure to allocate memory\n");
            return 1;
        }

        srand(time(NULL));
        generateBalls(numBalls, balls, &borderBall);

        while (!exit) {
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(renderer);

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

                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
                drawCircle(renderer, borderBall.position.x, borderBall.position.y, borderBall.radius);
                drawCircle(renderer, balls[i].position.x, balls[i].position.y, balls[i].radius);
            }


            // Update screen
            SDL_RenderPresent(renderer);

            // Delay for approx. 30 fps
            SDL_Delay(32) ; // 32 ms
        }

        closeApp();
    }

    return 0;
}
