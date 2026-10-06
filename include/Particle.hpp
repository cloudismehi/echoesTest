#include <iostream> 
#include <raylib.h>
#include <raymath.h>

#include "include/Room.hpp"

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