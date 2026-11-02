#include "include/Particle.hpp"

Particle::Particle(Room &_room){
    room = &_room; 
    particlePosition = (*room).emiterPosition; 
}

Vector2 Particle::calculateRayPaths(int maxCollisions, Vector2 angle){
    // overwrite particle's parameters with function inputs
    exitAngle = angle; 

    /*
    recalculate the particle velocity with new exit angle by converting spherical coordinates {stepOffset, angle.x, angle.y}
    into rectangular coordinates
    */ 
    particleVelocity.x = stepOffset * std::sin(angle.y) * std::cos(angle.x); 
    particleVelocity.y = stepOffset * std::sin(angle.y) * std::sin(angle.x); 
    particleVelocity.z = stepOffset * std::cos(angle.y); 

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
    Vector3 distanceTally = {0,0,0}; 

    /*
    failsafe in case the while loop runs for too long, the limit may not account for all possible real scenarios, more 
    like a sanity check that the while loop is getting a little crazy 
    */ 
    int iterations = 0; 
    int maxIterations = 1000000; 

    // keeps track of reflectivity per collision 
    float reflectivity = 1.f; 

    //resets the collision history and adds the original position for each new particle run
    collisionHistory.clear(); 
    collisionHistory.push_back(particlePosition); 

    if (collisionMessages){
        std::cout << "---------------------\n"; 
        std::cout << "initial position: (" << particlePosition.x << ", " << particlePosition.y << ", " << particlePosition.z << ")\n"; 
        std::cout << "initial velocity: (" << particleVelocity.x << ", " << particleVelocity.y << ", " << particleVelocity.z << ")\n"; 
    }

    while (runLoop){
        if (iterations++ > maxIterations){
            std::cout << "loop exited due to limit reached, final particle position (" << particlePosition.x << ", " << particlePosition.y << ")\n"; 
            break; 
        }
        particlePosition += particleVelocity; // update particle position
        /*
        check boundaries, reverse velocity and stop loop if boundaries are exeeded
        */
        if (particlePosition.x >= (*room).roomScale_px.x){ // right wall 
            particleVelocity.x *= -1; 
            collisionDetected = true;  
            reflectivity *= (*room).refWallRight; 
        } else if (particlePosition.x <= 0){ // left wall
            particleVelocity.x *= -1; 
            collisionDetected = true;        
            reflectivity *= (*room).refWallLeft; 
        }
        else if (particlePosition.y >= (*room).roomScale_px.y){ // wall front
            particleVelocity.y *= -1; 
            collisionDetected = true; 
            reflectivity *= (*room).refWallFront; 
        } else if (particlePosition.y <= 0){ // wall back
            particleVelocity.y *= -1; 
            collisionDetected = true;        
            reflectivity *= (*room).refWallBack; 
        }
        else if (particlePosition.z >= (*room).roomScale_px.z){ // ceiling
            particleVelocity.z *= -1; 
            collisionDetected = true; 
            reflectivity *= (*room).refCeiling; 
        } else if (particlePosition.z <= 0){ // floor
            particleVelocity.z *= -1; 
            collisionDetected = true;        
            reflectivity *= (*room).refFloor;
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
            distanceTally += {std::abs(particlePosition.x - collisionHistory.back().x), std::abs(particlePosition.y - collisionHistory.back().y), 
                std::abs(particlePosition.z - collisionHistory.back().z)}; 
           
            collisionHistory.push_back(particlePosition); // update history 

            // print the collision position, for debugging
            if (collisionMessages){
                std::cout << "collision with boundary! at (" << particlePosition.x << ", " << particlePosition.y << ", ";
                std::cout << particlePosition.z << ")\n"; 
            }

            // max collisions reached
            if (collisions >= maxCollisions){
                runLoop = false; 
                // add last leg of travel of the ray to the microphone
                distanceTally += {std::abs(particlePosition.x - (*room).microphonePosition.x), std::abs(particlePosition.y - (*room).microphonePosition.y), 
                    std::abs(particlePosition.z - (*room).microphonePosition.z)}; 
                
                // print final collision with microphone, for debugging 
                if (collisionMessages){
                    std::cout << "collision with microphone! at (" << (*room).microphonePosition.x << ", " << (*room).microphonePosition.y << ", ";
                    std::cout << (*room).microphonePosition.z << ")\n"; 
                }

                // convert distance in pixels to distance in meters
                distanceTally.x /= (*room).scale.x; 
                distanceTally.y /= (*room).scale.y; 
                distanceTally.z /= (*room).scale.z; 
                
                // reset particle position for reuse 
                particlePosition = (*room).emiterPosition;

                return {Vector3Length(distanceTally), reflectivity}; // return distance
            }
        }
    }
    if (collisionMessages) std::cout << "---------------------\n"; 
    return {0, 0}; 
}

std::vector<Vector3> Particle::calculateRadialRayPaths(Vector2 numberOfRays, int maxCollisions)
{
    // all the delay times and distances will be stored here, the size of this vector depends on numberOfRays
    std::vector<Vector3> delayParams; 
    float delayTime = 0; 
    float delayDistance = 0; 
    // a holder for the output of the calculateRayPaths function which returns {distance, reflectivity}
    Vector2 distanceAndAttenuation = {0, 0}; 
    
    // angle difference between rays in radians 
    float deltaAngle_xy = (2*PI) / numberOfRays.x; 
    float deltaAngle_z = PI / (numberOfRays.y + 1);

    // angle of emission in radians, start at zero
    float angle_xy = 0;

    //total reflectivity from collisions as the boundary's reflectivity factor to the power of the number of collisions
    float att = 1.f; 

    // calculate delay fully pointing up from emitter
    distanceAndAttenuation = calculateRayPaths(maxCollisions, {0, 0}); 
    delayDistance = distanceAndAttenuation.x; 
    delayTime = calculateTravelTime(delayDistance); 
    att = distanceAndAttenuation.y; 
    // push initial parameter into vector
    delayParams.push_back({delayTime, delayDistance, att}); 

    // Add the requested number of azimuth samples at each interior z-plane.
    for (int q = 1; q <= numberOfRays.y; q++){
        float angle_z = q * deltaAngle_z;

        // sweep through xy plane for each angle in z 
        for (int i = 0; i < numberOfRays.x; i++){
            angle_xy = i * deltaAngle_xy;

            // calculate ray times for given emission angle
            distanceAndAttenuation = calculateRayPaths(maxCollisions, {angle_xy, angle_z}); 
            delayDistance = distanceAndAttenuation.x; 
            delayTime = calculateTravelTime(delayDistance); 
            att = distanceAndAttenuation.y; 
            // push azimuthal parameters into vector
            delayParams.push_back({delayTime, delayDistance, att}); 
        }
    }
    // calculate delay fully pointing down from emitter
    distanceAndAttenuation = calculateRayPaths(maxCollisions, {0, PI}); 
    delayDistance = distanceAndAttenuation.x; 
    delayTime = calculateTravelTime(delayDistance); 
    att = distanceAndAttenuation.y; 
    // push final down parameter into vector 
    delayParams.push_back({delayTime, delayDistance, att}); 

    return delayParams;
}

float Particle::calculateTravelTime(float distance){
    return distance / particleSpeed; 
}