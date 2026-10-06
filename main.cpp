#include <iostream> 
#include <vector>
#include <raylib.h>
#include <raymath.h>

class Room{
    public: 
    /*
    for both scaling terms, the minimum is assumed to be (0, 0), so we won't need to define them as well - 
    the room is fully defined by the maximum points in x and y. 
    */
    
    // scale of the room in pixels
    Vector2 roomScale_px = {300, 300}; 
    // scale of the room in meters
    Vector2 roomScale_m = {1, 1};
    //scale between pixels and meters [px/m], used for conversion later
    Vector2 scale = {(roomScale_px.x / roomScale_m.x), (roomScale_px.y / roomScale_m.y)}; 
    
    /*
    position of the emitter (sound source) and the microphone (sound sink)
    they're both in pixels and will eventually be turned into meters when the calculations in the particle are all done
    */
    Vector2 emiterPosition = {5, roomScale_px.y/2}; 
    Vector2 microphonePosition = {roomScale_px.x - 5, roomScale_px.y/2}; 
}; 

class Particle{
    private: 
    // copy of the room for values needed in internal calculations (room dimensions, microphone and emitter positions, etc.)
    Room *room; 
    // flag to turn on and off the debugging messages of where each collision happened
    bool collisionMessages = true; 
    public: 

    // overall velocity magnitude, used when calculating delay time [in meters per second, i guess] 
    float speed = 10; 
    // simulation offset in the steps, how many pixels are skipped per iteration
    float stepOffset = 1; 
    // angle (in radians) from the x-axis (x_pos to the right) with which particle is moving through the room 
    float exitAngle = 0; 

    /*
    particle velocity, calculated with the speed scaled by the cosine and sine of the exit angle 
    note: there is a negative sign on the y-component of the velocity because in graphics, y goes higher as it descends on the screen, my math's
    y-axis is positive going up - that negative sign keeps those two coherent 
    */ 
    Vector2 particleVelocity = {0,0}; 

    // initial position of the particle is equal to the position of the emitter defined in the room class
    Vector2 particlePosition = {0, 0}; 

    // history of all previous collision points, arranged from oldest to newest, not by design but because of how cpp vectors work
    std::vector<Vector2> collisionHistory; 
    
    // class constructor, copy over the room details of the room
    Particle(Room &_room);

    /*
    run the loop that steps the ray around the world searching for collisions
    save collision position on vector, and print distance. 
    parameters: max number of collisions (default: 1), this does not count the "collision" with the microphone
    */
    float calculateRayPaths(int maxCollisions = 1, float angle = 0); 
}; 


int main(){
    Room room; 
    Particle particle(room); 

    std::cout << "distance: " << particle.calculateRayPaths(2) << '\n'; 
    std::cout << std::endl; 
    std::cout << "distance: " << particle.calculateRayPaths(2, PI/2) << '\n'; 
    std::cout << std::endl; 
    std::cout << "distance: " << particle.calculateRayPaths(2, PI) << '\n'; 
    std::cout << std::endl; 
    std::cout << "distance: " << particle.calculateRayPaths(2, 3 * PI/2) << '\n'; 
    
    return 0; 
}

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