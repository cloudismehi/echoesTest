#include <iostream> 
#include <vector>
#include <raylib.h>
#include <raymath.h>
#include <fstream> 
#include <chrono>

#include "include/Room.hpp"
#include "include/Particle.hpp"
#include "include/DelayLine.hpp"
#include "include/SineWave.hpp"
#include "include/ProjectInfo.hpp"


int main(){
    InitAudioDevice(); 
    ProjectInfo projectInfo; 
    Room room; 
    Particle particle(room); 
    
    auto reverberationParams = particle.calculateRadialRayPaths(projectInfo.numberOfRays, 5); 
    DelayLine delayLine(projectInfo.totalNumberOfDelays, reverberationParams); 
    
    Wave inputWave = LoadWave("assets/cheers.wav"); 
    float* inputDump = LoadWaveSamples(inputWave); 
    int numSamples = inputWave.frameCount * inputWave.channels; 
    
    std::fstream outfile("assets/cheersRev.raw", std::ios::out | std::ios::binary);  

    for (int i = 0; i < numSamples; i++){
        auto output = delayLine.feedArr(inputDump[i] * 32767); 
        if (output.size() != 0){
            for (auto val : output){
                short buffer = (short) val; 
                outfile.write(reinterpret_cast<const char*> (&buffer), sizeof(buffer)); 
            }
        } 
    }

    outfile.close();

    // //! ONLY FOR DEBUGGING: turn raw file into wav file using ffmpeg, only works on my machine with my current 
    // //! project architecture
    system("ffmpeg -f s16le -ar 44100 -ac 1 -i assets/cheersRev.raw assets/cheersRev.wav"); 
    
    UnloadWaveSamples(inputDump); 
    UnloadWave(inputWave); 
    CloseAudioDevice(); 
    return 0; 
}
