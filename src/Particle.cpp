#include "include/Particle.hpp"

Particle::Particle(Room &_room){
    room = &_room; 
    particlePosition = (*room).emiterPosition; 
}

float Particle::calculateRayPaths(int maxCollisions, float angle, bool _returnTime){
    // overwrite particle's parameters with function inputs
    exitAngle = angle; 
    returnTime = _returnTime; 

    // recalculate the particle velocity with new exit angle
    particleVelocity = {stepOffset * std::cos(exitAngle), -stepOffset * std::sin(exitAngle)}; 

    // final check for when loop should stop
    bool runLoop = true; 
    
    // turns on briefly every time there is a collision, a flag to trigger collision code
    bool collisionDetected = false; 
    
    // keeps track of number of collisions to trigger end code when number exeeds maxCollisions
    int collisions = 0; 

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

                // check if we must return total time or distance, depends on boolean on Particle's private member
                if (returnTime){
                    return Vector2Length(distanceTally) / particleSpeed; // return time
                } else {
                    return Vector2Length(distanceTally); // return distance
                }
                
            }
        }
    }
    return 0; 
}

std::vector<float> Particle::calculateRadialRayPaths(int numberOfRays, int maxCollisions)
{
    // all the delay times will be stored here, the size of this vector depends on numberOfRays
    std::vector<float> delayTimes; 
    
    // angle difference between rays in radians 
    float deltaAngle = (2*PI) / numberOfRays; 

    // angle of emission in radians, start at zero
    float angle = 0; 

    for (int i = 0; i < numberOfRays; i++){
        // calculate ray times for given emission angle
        delayTimes.push_back(calculateRayPaths(maxCollisions, angle, true)); 

        // step angle up by angle offset
        angle += (deltaAngle); 
    }

    return delayTimes;
}
