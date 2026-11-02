#include "include/SineWave.hpp"

short SineWave::sineWave(double time, double freq, double amp){
    short result; 
    // ticks per cycle
    double tpc = projectInfo.sampleRate / freq; 
    double numCycles = time / tpc; 
    double radians = twopi * numCycles; 
    // make sure the input amplitude's absolute value is not bigger than one
    double ampSafe = ((amp > 1) or (amp < -1)) ? 1 : amp; 
    // turn amplitude into a signed 16-bit amplitude
    short amplitude = 32767 * ampSafe; 
    // calculate result and implicitly recast into a short
    result = amplitude * sin(radians); 
    return result; 
}

void SineWave::generateSine(double freq, double amp, double length, short output[])
{   
    // generate a length seconds long array with the sine wave, the output array is part of the list of parameters
    for (int i = 0; i < (length*projectInfo.sampleRate); i++){
        output[i] = sineWave(i, 200, 0.2); 
    }
    // generate the file if wanted
    if (generateRawFile){
        std::ofstream outFile("assets/rawSine.raw", std::ios::out | std::ios::binary); 
        outFile.write(reinterpret_cast<const char*>(output), 44100 * sizeof(short)); 
        outFile.close(); 
    }
}

void SineWave::generateSineChirp(double freq, double amp, double length, double chirpLength, short output[]){
    // generate a length seconds long array with the sine wave, the output array is part of the list of parameters
    for (int i = 0; i < (length*projectInfo.sampleRate); i++){
        // only generate a sine wave during the chirp-time, if not just output a zero
        if (i < (chirpLength*projectInfo.sampleRate))
            output[i] = sineWave(i, 200, 0.2); 
        else 
            output[i] = 0; 
    }
    // generate file if wanted
    if (generateRawFile){
        std::ofstream outFile("assets/rawSine.raw", std::ios::out | std::ios::binary); 
        outFile.write(reinterpret_cast<const char*>(output), 44100 * sizeof(short)); 
        outFile.close(); 
    }
}
