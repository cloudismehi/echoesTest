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
    absorptions factors for collisions with walls and ceiling/floor, there are only four walls and one ceiling and one floor
    so there will be six absorption factors here, as the project scales the hope is to add a way to add/remove these 
    factors during runtime. these are separated by band on the frequencies: 
    {125, 250, 500, 1k, 2k, 4k}
    the source for these factors is: 
    https://commercial-acoustics.com/sound-advice/sound-absorption-coefficient-chart/
    */
   float absCeiling[6] = {0.24, 0.19, 0.14, 0.08, 0.13, 0.1};       // wood
   float absFloor[6] = {0.01, 0.02, 0.06, 0.15, 0.25, 0.45};        // carpet
   float absWallLeft[6] = {0.29, 0.10, 0.06, 0.05, 0.04, 0.04};     // drywall
   float absWallRight[6] = {0.29, 0.10, 0.06, 0.05, 0.04, 0.04};    // drywall
   float absWallFront[6] = {0.29, 0.10, 0.06, 0.05, 0.04, 0.04};    // drywall
   float absWallBack[6] = {0.29, 0.10, 0.06, 0.05, 0.04, 0.04};     // drywall

   /*
   absorption factor for sound travelling through space given in a factor per frequency range
   {125, 250, 500, 1k, 2k, 4k}
   in terms of db/km
   the source for these factors is the ISO standard 9613-1:1993 at T = 15C, P = 1atm, relative humidity = 50% (picked arbirtrarily)
   */
  float airPropagation[6] = {0.479, 1.22, 2.24, 4.16, 10.8, 36.2}; 
}; 