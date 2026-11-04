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


int main(){
    ProjectInfo projectInfo; 
    Room room; 
    Particle particle(room); 
    WhiteNoise whiteNoise; 
    short out[44100]; 
    
    whiteNoise.generateNoiseBurst(0.2, 1, 0.5, out); 

    float arr[] = {0, 0, 0, 0, 0, 1}; 
    DelayUnit unit(1, arr); 

    // auto reverberationParams = particle.calculateRadialRayPaths(projectInfo.numberOfRays, 5); 
    
    // DelayLine delayLine(projectInfo.totalNumberOfDelays, reverberationParams); 
    // delayLine.printDelayInfo(); 

    std::fstream outfile("assets/reverbRawSine.raw", std::ios::out | std::ios::binary); 
    
    for (int i = 0; i < 44100; i++){
        auto output = unit.feedArr(out[i]); 
        if (output.size() != 0){
            for (auto val : output){
                short foo = (short) val; 
                outfile.write(reinterpret_cast<const char*> (&foo), sizeof(foo)); 
            }
        }
    }

    // for (int i = 0; i < 44100; i++){
    //     auto output = delayLine.feedArr(out[i]); 
    //     if (output.size() != 0){
    //         for (auto val : output){
    //             short foo = (short) val; 
    //             outfile.write(reinterpret_cast<const char*> (&foo), sizeof(foo)); 
    //         }
    //     } 
    // }

    outfile.close();


    // //! ONLY FOR DEBUGGING: turn raw file into wav file using ffmpeg, only works on my machine with my current 
    // //! project architecture
    system("ffmpeg -f s16le -ar 44100 -ac 1 -i assets/reverbRawSine.raw assets/reverbSine.wav"); 

    return 0; 
}
