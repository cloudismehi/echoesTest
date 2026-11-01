#pragma once

#include <iostream> 
#include <raylib.h>
#include <raymath.h> 

class Room{
    public: 
    /*
    for both scaling terms, the minimum is assumed to be (0, 0), so we won't need to define them as well - 
    the room is fully defined by the maximum points in x and y. 
    */
    
    // scale of the room in pixels
    Vector3 roomScale_px = {300, 300, 300}; 
    // scale of the room in meters
    Vector3 roomScale_m = {10, 10, 10};
    //scale between pixels and meters [px/m], used for conversion later
    Vector3 scale = {(roomScale_px.x / roomScale_m.x), (roomScale_px.y / roomScale_m.y), (roomScale_px.z / roomScale_m.z)}; 
    
    /*
    position of the emitter (sound source) and the microphone (sound sink)
    they're both in pixels and will eventually be turned into meters when the calculations in the particle are all done
    */
    Vector3 emiterPosition = {5, roomScale_px.y/2, 150}; 
    Vector3 microphonePosition = {roomScale_px.x - 5, roomScale_px.y/2, 150}; 

    /*
    reflectivity factors for collisions with walls and ceiling/floor, there are only four walls and one ceiling and one floor
    so there will be six reflectivity factors here, as the project scales the hope is to add a way to add/remove these 
    factors during runtime
    */
   float refCeiling = 0.93; // wood
   float refFloor = 0.63; // carpet 
   float refWallLeft = 0.9; // plywood paneling
   float refWallRight = 0.9; // plywood paneling
   float refWallFront = 0.9; // plywood paneling
   float refWallBack = 0.9; // plywood paneling
//    float refCeiling = 1; // wood
//    float refFloor = 1; // carpet 
//    float refWallLeft = 1; // plywood paneling
//    float refWallRight = 1; // plywood paneling
//    float refWallFront = 1; // plywood paneling
//    float refWallBack = 1; // plywood paneling
}; 