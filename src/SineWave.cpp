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


// Initialize random engine with a non-deterministic seed, and map values between -1.0 and 1.0
WhiteNoise::WhiteNoise() : rng(std::random_device{}()), dist(-1.0, 1.0) {}

short WhiteNoise::noiseSample(double amp) {
    // Generate a random float between -1.0 and 1.0
    double randomValue = dist(rng);
    
    // Safety check the user's input amplitude boundaries
    double ampSafe = ((amp > 1) || (amp < -1)) ? 1 : amp; 
    
    // Scale the raw -1.0 to 1.0 value by the requested 16-bit safe amplitude
    short amplitude = 32767 * ampSafe; 
    
    return static_cast<short>(amplitude * randomValue); 
}

void WhiteNoise::generateNoise(double amp, double length, short output[]) {   
    size_t totalSamples = static_cast<size_t>(length * projectInfo.sampleRate);

    // Populate the buffer sample-by-sample
    for (size_t i = 0; i < totalSamples; i++) {
        output[i] = noiseSample(amp); 
    }
    
    // Generate the raw file mirroring your exact asset layout
    if (generateRawFile) {
        std::ofstream outFile("assets/rawNoise.raw", std::ios::out | std::ios::binary); 
        outFile.write(reinterpret_cast<const char*>(output), totalSamples * sizeof(short)); 
        outFile.close(); 
    }
}

void WhiteNoise::generateNoiseBurst(double amp, double length, double burstLength, short output[]) {
    size_t totalSamples = static_cast<size_t>(length * projectInfo.sampleRate);
    size_t burstSamples = static_cast<size_t>(burstLength * projectInfo.sampleRate);

    for (size_t i = 0; i < totalSamples; i++) {
        // Output noise during burst window, otherwise clamp strictly to zero (silence)
        if (i < burstSamples)
            output[i] = noiseSample(amp); 
        else 
            output[i] = 0; 
    }
    
    if (generateRawFile) {
        std::ofstream outFile("assets/rawNoise.raw", std::ios::out | std::ios::binary); 
        outFile.write(reinterpret_cast<const char*>(output), totalSamples * sizeof(short)); 
        outFile.close(); 
    }
}
