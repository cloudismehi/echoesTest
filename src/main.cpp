#include <iostream> 
#include <vector>
#include <raylib.h>
#include <raymath.h>

#include "include/Room.hpp"
#include "include/Particle.hpp"

std::vector<float[2]> reverberationParams; 

int main(){
    Room room; 
    Particle particle(room); 

    auto delayTimes = particle.calculateRadialRayPaths({4, 4}, 2); 
    for (auto time : delayTimes){
        std::cout << time << '\n'; 
    }

    return 0; 
}

