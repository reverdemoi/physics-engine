
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