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

    std::cout << "manual calculation results: \n"; 
    std::cout << "time: " << particle.calculateRayPaths(2) << '\n'; 
    std::cout << "time: " << particle.calculateRayPaths(2, PI/2) << '\n'; 
    std::cout << "time: " << particle.calculateRayPaths(2, PI) << '\n'; 
    std::cout << "time: " << particle.calculateRayPaths(2, 3 * PI/2) << '\n'; 
    
    auto delayTimes = particle.calculateRadialRayPaths(4, 2); 
    std::cout << "\nautomatic calculation results:\n"; 
    for (auto delay : delayTimes){
        std::cout << delay << '\n'; 
    }


    return 0; 
}

