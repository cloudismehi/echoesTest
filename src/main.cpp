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
std::vector<Vector3> reverberationParams; 


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
    
    reverberationParams = particle.calculateRadialRayPaths(projectInfo.numberOfRays, 5); 
    
    DelayLine delayLine(projectInfo.totalNumberOfDelays, reverberationParams); 

    std::fstream outfile("assets/reverbRawSine.raw", std::ios::out | std::ios::binary); 
    short reverbOut[44100]; 
    for (int i = 0; i < 44100; i++){
        reverbOut[i] = (short) (delayLine.feed((double) out[i]) * projectInfo.wetRev) + out[i]; 
    }

    outfile.write(reinterpret_cast<const char*> (reverbOut), 44100 * sizeof(short)); 
    outfile.close();

    delayLine.printDelayInfo(); 

    //! ONLY FOR DEBUGGING: turn raw file into wav file using ffmpeg, only works on my machine with my current 
    //! project architecture
    system("ffmpeg -f s16le -ar 44100 -ac 1 -i assets/reverbRawSine.raw assets/reverbSine.wav"); 

    return 0; 
}
