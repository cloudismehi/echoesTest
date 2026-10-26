#include <iostream> 
#include <vector>
#include <raylib.h>
#include <raymath.h>

#include "include/Room.hpp"
#include "include/Particle.hpp"

std::vector<Vector2> reverberationParams; 

int main(){
    Room room; 
    Particle particle(room); 

    reverberationParams = particle.calculateRadialRayPaths({4, 4}, 2); 
    int i = 0; 
    for (auto time : reverberationParams){
        std::cout << "(" << i + 1 << ") " << time.x << ", " << time.y << '\n'; 
        i++; 
    }

    return 0; 
}

