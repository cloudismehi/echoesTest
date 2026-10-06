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