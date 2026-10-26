#pragma once 
// header file with all the important project settings needed accross classes and files

#include <raylib.h> 

class ProjectInfo{
    public: 
    int sampleRate = 44100; // in kHz
    // {number of azimuth samples per z-plane, number of interior z-planes}
    Vector2 numberOfRays = {4, 4};
    // (+ 2) to account for the rays going straight up and straight down
    int totalNumberOfDelays = (numberOfRays.x * numberOfRays.y) + 2; 
}; 