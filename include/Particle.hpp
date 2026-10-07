#include <iostream> 
#include <vector>
#include <raylib.h>
#include <raymath.h>

#include "include/Room.hpp"

class Particle{
    private: 
    // copy of the room for values needed in internal calculations (room dimensions, microphone and emitter positions, etc.)
    Room *room; 
    // flag to turn on and off the debugging messages of where each collision happened
    bool collisionMessages = false; 
    // flag to toggle wether the calculateRayPaths function returns length or time
    bool returnTime = true; 
    public: 

    // overall velocity magnitude, used when calculating delay time [in meters per second] 
    float particleSpeed = 343; // speed of sound
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
    save collision position on vector, and print distance if option is turned on in particle private member. 
    parameters: max number of collisions (default: 1), this does not count the "collision" with the microphone
    output: [float]length or time of total raypaths, depending on setting on particle's private member
    */
    float calculateRayPaths(int maxCollisions = 1, float angle = 0, bool _returnTime = true); 

    /*
    calculate ray paths for numberOfRays rays equidistantly spread out from the source
    parameters: number of rays extending out from the source (default: 4), number of max collisions per ray
    return: [float vector] array of all collision times
    */
    std::vector<float> calculateRadialRayPaths(const int numberOfRays = 4, int maxCollisions = 1); 
}; 