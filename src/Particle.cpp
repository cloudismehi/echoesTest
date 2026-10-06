#include "include/Particle.hpp"

Particle::Particle(Room &_room){
    room = &_room; 
    particlePosition = (*room).emiterPosition; 
}

float Particle::calculateRayPaths(int maxCollisions, float angle){
    exitAngle = angle; 
    particleVelocity = {stepOffset * std::cos(exitAngle), -stepOffset * std::sin(exitAngle)}; 
    bool runLoop = true; // final check for when loop should stop
    bool collisionDetected = false; // turns on briefly every time there is a collision, a flag to trigger collision code
    int collisions = 0; // keeps track of number of collisions to trigger end code
    /*
    keeps track of distances between collisions, this is needed because the distance calculated won't take into account 
    more than the distance between start and end points, without calculating the distance after collisions. 
    */
    Vector2 distanceTally = {0,0}; 

    /*
    failsafe in case the while loop runs for too long, the limit may not account for all possible real scenarios, more 
    like a sanity check that the while loop is getting a little crazy 
    */ 
    int iterations = 0; 
    int maxIterations = 1000000; 

    //resets the collision history and adds the original position for each new particle run
    collisionHistory.clear(); 
    collisionHistory.push_back(particlePosition); 

    if (collisionMessages)
        std::cout << "initial position: (" << particlePosition.x << ", " << particlePosition.y << ")\n"; 

    while (runLoop){
        if (iterations++ > maxIterations){
            std::cout << "loop exited due to limit reached, final particle position (" << particlePosition.x << ", " << particlePosition.y << ")\n"; 
            break; 
        }
        particlePosition += particleVelocity; // update particle position
        /*
        check boundaries, reverse velocity and stop loop if boundaries are exeeded
        */
        if ((particlePosition.x >= (*room).roomScale_px.x) || (particlePosition.x <= 0)){
            particleVelocity.x *= -1; 
            collisionDetected = true;        
        } else if ((particlePosition.y >= (*room).roomScale_px.y) || (particlePosition.y <= 0)){
            particleVelocity.y *= -1; 
            collisionDetected = true; 
        }
        // if collision has been found, then calculate the distance traveled and return a report 
        if (collisionDetected){
            collisions++; 
            collisionDetected = false; // reset collision flag

            
            /*
            calculate distance traveled on this last frame in meters
            note: we have to calculate against the last entry in the collision history [collisionHistory.back()] to keep track 
            of a total of the movement *between* frames
            */
            distanceTally += {std::abs(particlePosition.x - collisionHistory.back().x), std::abs(particlePosition.y - collisionHistory.back().y)}; 
           
            collisionHistory.push_back(particlePosition); // update history 

            // print the collision position, for debugging
            if (collisionMessages)
                std::cout << "collision detected! at (" << particlePosition.x << ", " << particlePosition.y << ")\n"; 

            // max collisions reached
            if (collisions >= maxCollisions){
                runLoop = false; 
                // add last leg of travel of the ray to the microphone
                distanceTally += {std::abs(particlePosition.x - (*room).microphonePosition.x), std::abs(particlePosition.y - (*room).microphonePosition.y)}; 
                
                // print final collision with microphone, for debugging 
                if (collisionMessages)
                    std::cout << "collision with microphone! at (" << (*room).microphonePosition.x << ", " << (*room).microphonePosition.y << ")\n"; 

                // convert distance in pixels to distance in meters
                distanceTally.x /= (*room).scale.x; 
                distanceTally.y /= (*room).scale.y; 
                
                // reset particle position for reuse 
                particlePosition = (*room).emiterPosition;

                // return the magnitude of the final distance 
                return Vector2Length(distanceTally);
            }
        }
    }
    return 0; 
}