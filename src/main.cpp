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

void runTest(int maxNoColl, int noRays_az, int noRays_int, std::ofstream& outCsv, float* input, int inputLength); 

int main(){
    InitAudioDevice(); 
    std::ofstream csvOutput("assets/benchmark.csv"); 
    Wave inputWave = LoadWave("assets/cheers.wav"); 
    float* inputDump = LoadWaveSamples(inputWave); 
    int numSamples = inputWave.frameCount * inputWave.channels; 

    csvOutput << "Number of collisions,No. particles (azimuth),No. particles (internal),reverbParams (s),writingAndDelay (s)\n";
    for (int i = 1; i < 200; i+= 20){
        std::cout << "running test with " << i << "/200 collisions\n";
        for (int q = 4; q < 100; q += 40){
            printf("    trying angle (%d, %d)\n", q, q); 
            runTest(i, q, q, csvOutput, inputDump, numSamples); 
        }
    }
    csvOutput.close(); 
    UnloadWaveSamples(inputDump); 
    UnloadWave(inputWave); 
    CloseAudioDevice(); 
    return 0; 
}

void runTest(int maxNoColl, int noRays_az, int noRays_int, std::ofstream& outCsv, float* input, int inputLength){
    Room room; 
    Particle particle(room); 
    
    auto beforeReverbParams = std::chrono::high_resolution_clock::now(); 

    auto reverberationParams = particle.calculateRadialRayPaths((Vector2){(float)noRays_az, (float)noRays_int}, maxNoColl); 
    int totalNumberOfDelays = (noRays_az * noRays_int) + 2; 
    DelayLine delayLine(totalNumberOfDelays, reverberationParams); 

    auto afterReverbParams = std::chrono::high_resolution_clock::now(); 
    
    std::fstream outfile("assets/cheersRev.raw", std::ios::out | std::ios::binary);  

    auto beforeWritingAndDelay = std::chrono::high_resolution_clock::now(); 
    for (int i = 0; i < inputLength; i++){
        auto output = delayLine.feedArr(input[i] * 32767); 
        if (output.size() != 0){
            for (auto val : output){
                short buffer = (short) val; 
                outfile.write(reinterpret_cast<const char*> (&buffer), sizeof(buffer)); 
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
       << reverbParams.count() << ',' << writingAndDelay.count() << '\n';
}
