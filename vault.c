
                // Keyboard controls
                // if (e.type == SDL_KEYDOWN) {
                //     switch(e.key.keysym.sym) {
                //         case SDLK_UP:
                //             circleY -= velY;
                //             velY += velMod;
                //             break;
                        
                //         case SDLK_DOWN:
                //             circleY += velY;
                //             velY += velMod;
                //             break;

                //         case SDLK_LEFT:
                //             circleX -= velX;
                //             velX += velMod;
                //             break;

                //         case SDLK_RIGHT:
                //             circleX += velX;
                //             velX += velMod;
                //             break;
                //     }
                // } else {
                //     velX = velMod; 
                //     velY = velMod;
                // }






                
            // circleY += velY;
            // velY += velMod; 

            // boarderCollision(&ball.position.x, &ball.position.y, &ball.velocity.x, &ball.velocity.y, ball.radius, &boarderBall.position.x, &boarderBall.position.y, boarderBall.radius);

            // DEBUG - Make ball follow mouse position
            // int mouseX, mouseY;
            // Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
            // circleX = mouseX;
            // circleY = mouseY;








            
            // check if stuck inside 
            /* 
            Vector normalVel1 = normalize(newVel1);
            Vector normalVel2 = normalize(newVel2);

            Vector newPos1 = add(ball->position, multiply(newVel1, TIME_STEP)); 
            Vector newPos2 = add(balls[j].position, multiply(newVel2, TIME_STEP));

            Ball newBall1 = {newPos1, newVel1, ball->radius};
            Ball newBall2 = {newPos2, newVel2, balls[j].radius};

            if (collisionCheck(&newBall1, &newBall2)) {
                // printf("BALL STUCK\n");
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
            */



/* ANGULAR VELOCITY */

// In handleBallCollisions()
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

    // applyRollingPhysics(ball, &balls[j]); 

// In applyRollingPhysics()
void applyRollingPhysics(Ball* ball, Ball* otherBall) {
    if (magnitude(ball->velocity) < VELOCITY_THRESHOLD) {
        printf("%f IS ROLLING AGAINST %f\n", ball->ballNumber, otherBall->ballNumber);

        Vector distanceVec = subtract(ball->position, otherBall->position);
        Vector tangent = {distanceVec.y, -distanceVec.x};  // Perpendicular to the distance vector
        tangent = normalize(tangent);
        printf("tangent: %f, %f\n", tangent.x, tangent.y);

        // Use angular velocity to influence rolling
        double rollingFriction = 0.5; // This value can be adjusted
        printf("angularVelocity: %f\n", ball->angularVelocity);
        ball->velocity = multiply(tangent, ball->angularVelocity);
        ball->velocity = multiply(ball->velocity, -1);

        double angularDamping = 0.99;
        ball->angularVelocity *= angularDamping;

        printf("\n");
    }
}

// In updateBalls()
ball->orientation += ball->angularVelocity * deltaTime;

// Ensure the orientation stays within 0 to 2*PI
if (ball->orientation >= 2.0 * M_PI) {
    ball->orientation -= 2.0 * M_PI;
} else if (ball->orientation < 0) {
    ball->orientation += 2.0 * M_PI;
}


// In genBallValues()
ball->angularVelocity = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
ball->orientation = ((double)rand() / RAND_MAX) * 2.0 * M_PI;

// In ball.h struct of ball
double angularVelocity; // radians per second
double orientation; // radians


/* FIX FOR SINGLE BALL JITTERING AT BOTTOM CENTER */
// this took me way too long to figure out and didnt have the heart to just scratch it so I'm leaving it here
if (round(ball->position.y) == 330 && round(ball->velocity.x * 10) / 10 == 0) {
    ball->velocity.x = 0;
    ball->velocity.y = 0;
} else {
    ball->velocity.y += pow(GRAVITY, 2) * deltaTime;
}

/* DEBUG IN newBall() */
int mouseX, mouseY;
Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
newBall->position.x = mouseX;
newBall->position.y = mouseY;
// newBall->position = (Vector){SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0};
newBall->velocity = (Vector){1, 1};
newBall->radius = 25;
newBall->gravity = true;
newBall->angularVelocity = ((double)rand() / RAND_MAX) * 2.0 - 1;
newBall->orientation = ((double)rand() / RAND_MAX) * 2.0 * M_PI;
newBall->ballNumber = balls->size;
