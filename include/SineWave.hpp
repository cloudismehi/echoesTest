#include <iostream> 
#include <fstream> 
#include <random>

#include "include/ProjectInfo.hpp"

// sine wave generator
class SineWave{
    ProjectInfo projectInfo; 

    public: 
    bool generateRawFile = true; 
    float twopi = 6.283185307; 

    // generates a sample of the sine wave, call everytime we want a sample
    short sineWave(double time, double freq, double amp); 

    /*
    generates sampleRate * length of a sine wave, where the length is given in seconds. if generateRawFile
    is true, it'll also generate a .raw file containing the sample, if not it'll just fill the output[] array 
    */ 
    void generateSine(double freq, double amp, double length, short output[]); 

    /*
    generates a sampleRate * length file with chirpLength seconds of it occupied by a sine wave and the rest just silence. 
    if generateRawFile is true, it'll also generate a .raw file containing the sample, if not it'll just fill the output[] array
    */
    void generateSineChirp(double freq, double amp, double length, double chirpLength, short output[]); 
}; 


// white noise generator
class WhiteNoise {
    ProjectInfo projectInfo; 
    
    // C++11 standard random engine and distribution
    std::mt19937 rng; 
    std::uniform_real_distribution<double> dist;

    public: 
    bool generateRawFile = true; 

    // Constructor to initialize the random number generator
    WhiteNoise();

    // generates a single sample of white noise
    short noiseSample(double amp); 

    /*
    generates sampleRate * length of white noise, where the length is given in seconds.
    fills the output[] array and optionally generates a .raw file
    */ 
    void generateNoise(double amp, double length, short output[]); 

    /*
    generates a sampleRate * length file with burstLength seconds occupied by noise and the rest just silence. 
    */
    void generateNoiseBurst(double amp, double length, double burstLength, short output[]); 
};
