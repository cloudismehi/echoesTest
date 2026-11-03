#include <iostream> 
#include <vector>
#include <raylib.h>
#include <raymath.h>

#include "include/Room.hpp"
#include "include/ProjectInfo.hpp"

class Particle{
    private: 
    // copy of the room for values needed in internal calculations (room dimensions, microphone and emitter positions, etc.)
    Room *room; 
    public: 
    // flag to turn on and off the debugging messages of where each collision happened
    bool collisionMessages = false; 

    // overall velocity magnitude, used when calculating delay time [in meters per second] 
    float particleSpeed = 343; // speed of sound
    // simulation offset in the steps, how many pixels are skipped per iteration
    float stepOffset = 1; 
    // {azimuth, zenith} angles (in radians) with which particle is moving through the room 
    Vector2 exitAngle = {0, 0}; 

    /*
    particle velocity, calculated with the speed scaled by the cosine and sine of the exit angle 
    note: there is a negative sign on the y-component of the velocity because in graphics, y goes higher as it descends on the screen, my math's
    y-axis is positive going up - that negative sign keeps those two coherent 
    */ 
    Vector3 particleVelocity = {0,0, 0}; 

    // initial position of the particle is equal to the position of the emitter defined in the room class
    Vector3 particlePosition = {0, 0, 0}; 

    // history of all previous collision points, arranged from oldest to newest, not by design but because of how cpp vectors work
    std::vector<Vector3> collisionHistory; 

    // enum for the boundaries to keep them consistent throughout the code
    enum boundaries {RIGHT_WALL, LEFT_WALL, FRONT_WALL, BACK_WALL, CEILING, FLOOR}; 

    float att[6] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f}; 
    
    // class constructor, copy over the room details of the room
    Particle(Room &_room);

    /*
    run the loop that steps the ray around the world searching for collisions
    save collision position on vector, and print distance if option is turned on in particle private member. 
    parameters: max number of collisions (default: 1), this does not count the "collision" with the microphone
    output: [ParticleInfo] a structure containing the time, distance, and attenuation[6] for six frequency ranges
    */
    float calculateRayPaths(int maxCollisions = 1, Vector2 angle = {0, 0}); 

    /*
    calculate ray paths for numberOfRays rays equidistantly spread out from the source. The x value gives the number
    of azimuth samples per z-plane; the y value gives the number of interior z-planes, in addition to one ray each
    straight up and straight down from the source.
    parameters: [Vector2] {number of azimuth samples per z-plane, number of interior z-planes} (default: 4 in xy and 4 in z), 
        [int] number of max collisions per ray
    return: [Vector3 vector] {delay time [ms], delay distance [m], attenuation}
    */
    std::vector<ParticleInfo> calculateRadialRayPaths(const Vector2 numberOfRays = {4, 4}, int maxCollisions = 1); 

    // little helper function that just returns the time of travel for a given distance
    float calculateTravelTime(float distance); 

    /*
    calculates the total attenuation on a sound coming from both the inverse square law and frequency-dependent 
    sound absorption
    */
    float calculateGeometricSpreading(float distance, int freqRangeIndex); 
}; 

