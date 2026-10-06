#include <iostream> 
#include <vector>
#include <raylib.h>
#include <raymath.h>

#include "include/Room.hpp"
#include "include/Particle.hpp"


int main(){
    Room room; 
    Particle particle(room); 

    std::cout << "distance: " << particle.calculateRayPaths(2) << '\n'; 
    std::cout << std::endl; 
    std::cout << "distance: " << particle.calculateRayPaths(2, PI/2) << '\n'; 
    std::cout << std::endl; 
    std::cout << "distance: " << particle.calculateRayPaths(2, PI) << '\n'; 
    std::cout << std::endl; 
    std::cout << "distance: " << particle.calculateRayPaths(2, 3 * PI/2) << '\n'; 
    
    return 0; 
}

