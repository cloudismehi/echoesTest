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

    float beta = (float)fftSize / projectInfo.sampleRate; 
    // writing the coefficients k for the filtering (just to make sure it gets applied to both sides)
    // first five indeces are beta * freq
    attenuationIndeces[0] = beta * 125;     
    attenuationIndeces[1] = beta * 250;     
    attenuationIndeces[2] = beta * 500;     
    attenuationIndeces[3] = beta * 1000;     
    attenuationIndeces[4] = beta * 2000;     
    // middle index is the nyquist index 
    attenuationIndeces[5] = projectInfo.fftSize / 2; 
    // the mirrored ones are the fft size minus the first five indices reversed
    attenuationIndeces[6] = projectInfo.fftSize - attenuationIndeces[4]; 
    attenuationIndeces[7] = projectInfo.fftSize - attenuationIndeces[3]; 
    attenuationIndeces[8] = projectInfo.fftSize - attenuationIndeces[2]; 
    attenuationIndeces[9] = projectInfo.fftSize - attenuationIndeces[1]; 
    attenuationIndeces[10] = projectInfo.fftSize - attenuationIndeces[0]; 
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
    if (delayIndex >= delayLength) { delayIndex = 0; } 

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
        for (int k = 0; k < fftSize; k++){
            // for all these checks i am checking both the index k and its mirror reflection about fftSize/2
            if ((k <= attenuationIndeces[0]) || k >= attenuationIndeces[10]){
                // first band
                fftOutput[k][0] *= attenuation[0]; 
                fftOutput[k][1] *= attenuation[0]; 
            }
            else if ((k <= attenuationIndeces[1]) || k >= attenuationIndeces[9]){
                // second band
                fftOutput[k][0] *= attenuation[1]; 
                fftOutput[k][1] *= attenuation[1];
            }
            else if ((k <= attenuationIndeces[2]) || k >= attenuationIndeces[8]){
                // third band
                fftOutput[k][0] *= attenuation[2]; 
                fftOutput[k][1] *= attenuation[2];
            }
            else if ((k <= attenuationIndeces[3]) || k >= attenuationIndeces[7]){
                // fourth band
                fftOutput[k][0] *= attenuation[3]; 
                fftOutput[k][1] *= attenuation[3];
            }
            else if ((k <= attenuationIndeces[4]) || k >= attenuationIndeces[6]){
                // fifth band
                fftOutput[k][0] *= attenuation[4]; 
                fftOutput[k][1] *= attenuation[4];
            } else{
                // sixth band
                fftOutput[k][0] *= attenuation[5]; 
                fftOutput[k][1] *= attenuation[5];
            }
        } // loop through fft output

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

std::vector<float> DelayLine::feedArr(float input){
    // holder for the functions output, when the output is not an empty vector
    std::vector<float> output(fftSize); 
    // keeps track of whether any delay unit actually returned something, if they did we need to output it
    bool flag_somethingWritten = false; 

    // iterate through all delay units
    for (int i = 0; i < numberOfDelayUnits; i++){
        // temporary holder of feed output, this is needed so that i can add the values of the delays to one single vector 
        std::vector<float> intermediateOut = delayLine[i].feedArr(input); 
        // only write to output if the intermediate out has content 
        if (intermediateOut.size() != 0){
            // iterate through entries of intermediate output and add to output
            for (int q = 0; q < fftSize; q++){
                output[q] += intermediateOut[q]; 
                flag_somethingWritten = true; // says we need to output the vector
            }
        }
    } 
    if (flag_somethingWritten){
        return output; 
    } else {
        return std::vector<float>(); 
    }
    return std::vector<float>(); 
}
