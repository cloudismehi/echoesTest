#include <iostream> 
#include <fstream> 

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