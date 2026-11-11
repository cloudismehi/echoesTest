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

void runTest(int maxNoColl, int noRays_az, int noRays_int, std::ofstream& outCsv, std::ofstream& outSignalCsv, float* input, int inputLength); 

int main(){
    SineWave wave; 
    std::ofstream csvOutput("assets/benchmark.csv"); 
    std::ofstream outputSignal("assets/outputSignal.csv"); 

    short input[44100]; 
    wave.generateSine(200, 0.2, 1, input); 
    int frameCount = 512; 
    float* trimmedInput = new float[frameCount]; 

    for (int i = 0; i < frameCount; i++){
        trimmedInput[i] = (float) input[i]; 
    }
    
    csvOutput << "No. collisions,No. particles (azimuth),No. particles (internal),reverbParams (s),writingAndDelay (s),No. samples processed\n";
    for (int i = 0; i < 100; i++){
        std::cout << "perfoming test number " << i << '\n'; 
        runTest(200, i, std::floor(i/2), csvOutput, outputSignal, trimmedInput, frameCount); 
    }

    csvOutput.close(); 
    outputSignal.close(); 
    delete[] trimmedInput; 
    return 0; 
}

void runTest(int maxNoColl, int noRays_az, int noRays_int, std::ofstream& outCsv, std::ofstream& outSignalCsv, float* input, int inputLength){
    Room room; 
    Particle particle(room); 
    int numSamplesProcessed = 0; 
    
    std::cout << "  > calculating reverb parameters\n"; 
    auto beforeReverbParams = std::chrono::high_resolution_clock::now(); 

    auto reverberationParams = particle.calculateRadialRayPaths((Vector2){(float)noRays_az, (float)noRays_int}, maxNoColl); 
    int totalNumberOfDelays = (noRays_az * noRays_int) + 2; 
    DelayLine delayLine(totalNumberOfDelays, reverberationParams); 

    auto afterReverbParams = std::chrono::high_resolution_clock::now(); 
    
    std::fstream outfile("assets/cheersRev.raw", std::ios::out | std::ios::binary);  

    std::cout << "  > writing delay\n"; 
    auto beforeWritingAndDelay = std::chrono::high_resolution_clock::now(); 
    for (int i = 0; i < inputLength; i++){
        auto output = delayLine.feedArr(input[i] * 32767); 
        if (output.size() != 0){
            for (auto val : output){
                numSamplesProcessed++; 
                short buffer = (short) val; 
                outfile.write(reinterpret_cast<const char*> (&buffer), sizeof(buffer)); 
                outSignalCsv << buffer << '\n'; 
            }
        } 
    }
    auto afterWritingAndDelay = std::chrono::high_resolution_clock::now(); 

    outfile.close();

    // //! ONLY FOR DEBUGGING: turn raw file into wav file using ffmpeg, only works on my machine with my current 
    // //! project architecture
    // system("ffmpeg -f s16le -ar 44100 -ac 1 -i assets/cheersRev.raw assets/cheersRev.wav"); 
    
    // calculate times taken in each process
    auto reverbParams = std::chrono::duration<double>(afterReverbParams - beforeReverbParams);
    auto writingAndDelay = std::chrono::duration<double>(afterWritingAndDelay - beforeWritingAndDelay);

    outCsv << maxNoColl << ',' << noRays_az << ',' << noRays_int << ','
       << reverbParams.count() << ',' << writingAndDelay.count() << ',' << numSamplesProcessed << '\n';
}
