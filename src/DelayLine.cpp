#include "include/DelayLine.hpp"

DelayUnit::DelayUnit(int length){
    delayLength = length; 
    cache = new (std::nothrow) double[delayLength]; 

    if (cache == nullptr){
        printf("error allocating memory for delay unit at length %d\n", length); 
    }
}

void DelayUnit::changeDelayTime(double newLength){ 
    delete[] cache; 
    delayLength = newLength; 
    cache = new double[delayLength]; 
}

double DelayUnit::feed(double input){
    // read cached value from n samples ago, cached value is. zero if it hasn't been written over before
    double output = cache[ptr]; 
    // store current value at the pointer, then advance the pointer by one
    cache[ptr++] = input; 
    // wrap pointer around when it exceeds the cache length
    if (ptr >= delayLength) { ptr = 0; }
    return output; 
}

void DelayUnit::printDelayLegnth(){
    std::cout << "unit has a delay of " << delayLength << " samples\n"; 
}

DelayLine::DelayLine(int _numberOfDelayUnits, std::vector<Vector2> _delaySettings){
    // copy settings from function parameter to class members
    numberOfDelayUnits = _numberOfDelayUnits; 
    delaySettings = _delaySettings; 

    // initiate all the delay units with the input info and append to delay line vector
    for (auto setting : delaySettings){
        DelayUnit unit(secondsToSamples(setting.x)); 
        delayLine.push_back(unit); 
    }
}


void DelayLine::printDelayTimes(){
    std::cout << "printing delay line status: " << numberOfDelayUnits << " units found\n"; 
    int index = 0; 
    for (auto setting : delaySettings){
        printf("(%d) -> %0.2fms\n", ++index, setting.x*1000); 
    }
}

float DelayLine::secondsToSamples(float seconds){
    return seconds * projectInfo.sampleRate; 
}

double DelayLine::feed(double input){
    double output = 0; 

    for (int i = 0; i < numberOfDelayUnits; i++){
        output += (delayLine[i].feed(input) * 1);
    }
    return output;
}
