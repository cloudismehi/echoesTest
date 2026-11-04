#include "include/DelayLine.hpp"

DelayUnit::DelayUnit(int length, float* att){
    delayLength = length; 
    delayCache.resize(length);
    fftCache.resize(fftSize); 
    for (int i = 0; i < 6; i++){
        attenuation[i] = att[i]; 
    }

    // create fftw buffers
    fftInput.reset(static_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * fftSize))); 
    fftOutput.reset(static_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * fftSize))); 
    if (!fftInput || !fftOutput){
        throw std::bad_alloc{}; 
    }

    // create fftw plans
    fftPlanForward.reset(fftw_plan_dft_1d(fftSize, fftInput.get(), fftOutput.get(), FFTW_FORWARD, FFTW_ESTIMATE)); 
    fftPlanBackward.reset(fftw_plan_dft_1d(fftSize, fftOutput.get(), fftInput.get(), FFTW_BACKWARD, FFTW_ESTIMATE)); 
    if (!fftPlanBackward || !fftPlanForward){
        throw std::runtime_error("failed to create FFTW plan"); 
    }
}

void DelayUnit::changeDelayTime(double newLength){ 
    delayLength = newLength; 
    delayCache.resize(newLength); 
}

double DelayUnit::feed(double input){
    // read cached value from n samples ago, cached value is. zero if it hasn't been written over before
    double output = delayCache[delayIndex]; 
    // store current value at the pointer, then advance the pointer by one
    // TODO: handle filtering here on the input
    delayCache[delayIndex++] = (input) * 0.2; 
    // wrap index around when it exceeds the delayCache length
    if (delayIndex >= delayLength) { delayIndex = 0; }
    return output; 
}

std::vector<float> DelayUnit::feedArr(float input){
    // save delayed signal in fft cache
    fftCache[fftIndex++] = delayCache[delayIndex];  
    // save new input in the delay cache
    delayCache[delayIndex++] = input; 
    // wrap index when it exceeds the delayCache length
    if (delayIndex > delayLength) { delayIndex = 0; } 

    // check if cache is full, if it is then it's ready to be filtered
    if (fftIndex == fftSize){
        // filtering
        fftIndex = 0; 
        // copy all the data into the fftw cache 
        for (int i = 0; i < fftSize; i++){
            fftInput[i][0] = fftCache[i]; 
            fftInput[i][1] = 0.0; 
        }
        // execute fft -> fftOutput
        fftw_execute(fftPlanForward.get()); 
        //? filtering on fftOutput
        // execute ifft -> fftInput
        fftw_execute(fftPlanBackward.get()); 
        // copy output into fftCache to return
        for (int i = 0; i < fftSize; i++){
            fftCache[i] = static_cast<float>(fftInput[i][0] / fftSize); 
        }
        
        return fftCache; 
    } else {
        return emptyVec; 
    }

    return emptyVec;
}

void DelayUnit::printDelayLegnth(){
    std::cout << "unit has a delay of " << delayLength << " samples\n"; 
}

DelayLine::DelayLine(int _numberOfDelayUnits, std::vector<ParticleInfo> _delaySettings){
    // copy settings from function parameter to class members
    numberOfDelayUnits = _numberOfDelayUnits; 
    delaySettings = _delaySettings; 

    // initiate all the delay units with the input info and append to delay line vector
    for (auto setting : delaySettings){
        delayLine.emplace_back((int)(setting.time * projectInfo.sampleRate), setting.attenuation);
    }
}


void DelayLine::printDelayTimes(){
    std::cout << "printing delay line times: " << numberOfDelayUnits << " units found\n"; 
    int index = 0; 
    for (auto setting : delaySettings){
        printf("(%d) -> %0.2fms\n", ++index, setting.time*1000); 
    }
}

void DelayLine::printDelayInfo(){
   std::cout << "printing delay line status: " << numberOfDelayUnits << " units found\n"; 
    int index = 0; 
    for (auto setting : delaySettings){
        printf("(%d) -> time: %0.2fms, distance: %0.2fm attenuation: ( ", ++index, setting.time*1000, setting.distance); 
        for (int i = 0; i < 6; i++){
            std::cout << setting.attenuation[i] << " "; 
        }
        std::cout << " )\n"; 
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
