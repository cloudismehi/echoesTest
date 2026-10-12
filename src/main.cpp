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

    std::cout << "distance: " << particle.calculateRayPaths(2, {0, 0}, false) << std::endl; 

    //TODO: i am not handling the three dimensional angle properly, fix the math 

    return 0; 
}

