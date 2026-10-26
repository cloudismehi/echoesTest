#include <iostream> 
#include <vector>
#include <raylib.h>
#include <raymath.h>

#include "include/Room.hpp"
#include "include/Particle.hpp"
#include "include/DelayLine.hpp"

std::vector<Vector2> reverberationParams; 

int main(){
    Room room; 
    Particle particle(room); 
    std::vector<Vector2> testDelaySettings; 
    
    testDelaySettings.push_back({1.f/44100, 1}); 
    testDelaySettings.push_back({5.f/44100, 1});

    DelayLine delayLine(2, testDelaySettings);
    
    delayLine.delayLine[0].printDelayLegnth(); 
    delayLine.delayLine[1].printDelayLegnth(); 
    
    for (int i = 0; i < 10; i++){
        int feed = (i == 0) ? 1 : 0; 
        auto output = delayLine.feed(feed); 
        std::cout << "fed: " << feed << ". output: " <<  output + feed << '\n'; 
    }

    reverberationParams = particle.calculateRadialRayPaths({4, 4}, 2); 
    
    return 0; 
}
