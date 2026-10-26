#include <iostream> 
#include <vector>
#include <raylib.h>
#include <raymath.h>
#include <fstream> 

#include "include/Room.hpp"
#include "include/Particle.hpp"
#include "include/DelayLine.hpp"
#include "include/SineWave.hpp"
#include "include/ProjectInfo.hpp"


ProjectInfo projectInfo; 
std::vector<Vector2> reverberationParams; 


int main(){
    Room room; 
    Particle particle(room); 
    SineWave sineWave; 

    short out[44100]; 
    sineWave.generateRawFile = true; 
    sineWave.generateSineChirp(200, 0.2, 1, 0.5, out); 
    
    float min = 9999; 
    float max = -9999; 
    for (int i = 0; i < 44100; i++){
        if (out[i] > max) max = out[i]; 
        if (out[i] < min) min = out[i]; 
    }
    printf("max: %0.2f, min %0.2f\n", max, min);
    
    reverberationParams = particle.calculateRadialRayPaths({4, 4}, 2); 
    
    DelayLine delayLine(projectInfo.totalNumberOfDelays, reverberationParams); 

    

    return 0; 
}
