#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>

// Constants
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
        window = SDL_CreateWindow("chattens", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

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

// Handle ball collision with the border
double borderCollision(Ball* ball, Ball* borderBall) {
    Vector distanceVec = subtract(ball->position, borderBall->position);
    double distance = magnitude(distanceVec);

    if (distance >= (borderBall->radius - ball->radius)) {
        printf("COLLISION ");

        Vector normal = normalize(distanceVec);
        double dot = dotProduct(ball->velocity, normal);
        Vector scaledNormal = multiply(normal, 2 * dot);
        ball->velocity = subtract(ball->velocity, scaledNormal);

        ball->velocity = multiply(ball->velocity, VELOCITY_MODIFIER);

        // DEBUG SUGGA MY DICKO
        printf("velX: %f, velY: %f, posX: %f, posY: %f\n", ball->velocity.x, ball->velocity.y, ball->position.x, ball->position.y);
    }

    return distance;
}

int main(int argc, char* argv[]) {
    bool exit = false;
    SDL_Event e;
    Uint32 lastTime = SDL_GetTicks();

    if (!init()) {
        printf("Failure to initialize\n");
    } else {
        Ball ball = {{SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT / 2}, {1, 2}, 10};
        Ball borderBall = {{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, {0, 0}, 100};

        while (!exit) {
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(renderer);

            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    exit = true;
                }

                if (e.type == SDL_MOUSEBUTTONDOWN) {
                    // DEBUG - Make ball follow mouse position
                    int mouseX, mouseY;
                    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
                    ball.position.x = SCREEN_WIDTH / 2;
                    ball.position.y = mouseY;
                    ball.velocity.y = 2;
                }
            }

            // Fine test of using delta time, didnt work :(
            // Uint32 currentTime = SDL_GetTicks();
            // double deltaTime = currentTime - lastTime / 1000;
            // lastTime = currentTime;

            // Collision thingin
            double distance = borderCollision(&ball, &borderBall);

            // Update ball position
            ball.position = add(ball.position, multiply(ball.velocity, TIME_STEP));

            // STOP BALL FROM BOUNCING IF IT IS STUCK IN MIDDLE WITH NO X VELOCITY¨
            if (round(distance) == 90 && round(ball.position.y) == 330 && round(ball.velocity.x * 10) / 100 == 0 ) {
                printf("velX: %f\n", round(ball.velocity.x * 10) / 100);
                ball.velocity.x = 0;
                ball.velocity.y = 0;
            } else {
                ball.velocity.y += GRAVITY * TIME_STEP;
            }

            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
            drawCircle(renderer, borderBall.position.x, borderBall.position.y, borderBall.radius);
            drawCircle(renderer, ball.position.x, ball.position.y, ball.radius);

            // Update screen
            SDL_RenderPresent(renderer);

            // Delay for approx, 60 fps
            SDL_Delay(32) ; // 16 ms
        }

        closeApp();
    }

    return 0;
}
