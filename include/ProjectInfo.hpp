#pragma once 
// header file with all the important project settings needed accross classes and files

#include <raylib.h> 

class ProjectInfo{
    public: 
    int sampleRate = 44100; // in kHz
    int fftSize = 2048; 
    // {number of azimuth samples per z-plane, number of interior z-planes}
    Vector2 numberOfRays = {4, 4};
    // (+ 2) to account for the rays going straight up and straight down
    int totalNumberOfDelays = (numberOfRays.x * numberOfRays.y) + 2; 
    // max number of collisions
    int maxNoCollisions = 5; 
    // how much reverb signal is present on output
    float wetRev = 1;
}; 

struct ParticleInfo{ 
    float time; 
    float distance; 
    float attenuation[6]; 

    void fill(float _time, float _delayDistance, float* _attenuation){
        time = _time; 
        distance = _delayDistance; 
        for (int i = 0; i < 6; i++){
        attenuation[i] = _attenuation[i]; 
    }
    }
}; 