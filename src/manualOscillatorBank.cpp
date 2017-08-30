//
//  manualOscillatorBank.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 30/08/2017.
//
//

#include "manualOscillatorBank.h"
#include "parametersControl.h"

manualOscillatorBank::manualOscillatorBank(int nOscillators) : baseIndexer(nOscillators){
    
    parameters->add(phasorIn.set("Phasor In", 0, 0, 1));
    parameters->add(manualInput.set("Manual In", 0, 0, 1));
    parameters->add(delay.set("Delay", 0, 0, 1000));
    parameters->add(output.set("Output", {}));
    
    phasorIn.addListener(this, &manualOscillatorBank::computeValues);
    manualInput.addListener(this, &manualOscillatorBank::manualInputChanged);
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::blueSteel);

    buffer.reserve(1000);
    
    bufferIndex = 0;
    oldPhasor = 0;
}

void manualOscillatorBank::manualInputChanged(float &f){
//    while(bufferIndex >= buffer.size()){
//        buffer.push_back(f);
//    }
//    buffer[bufferIndex] = f;
//    ofLog() << buffer.size() << " " << bufferIndex;
}

void manualOscillatorBank::computeValues(float &f){
    bufferIndex++;
    if(oldPhasor > f) bufferIndex = 0;
    
    if(bufferIndex >= buffer.size()){
        buffer.push_back(manualInput);
    }else{
        buffer[bufferIndex] = manualInput;
    }
    
    oldPhasor = f;
    vector<float>   tempOut;
    tempOut.resize(indexs.size(), 0);
    
    int auxiliaryIndex = -1;
    
    for(int i = 0; i < indexs.size(); i++){
        int newBuffIndex = bufferIndex - int(indexs[i]*delay*indexs.size());
        int auxiliaryNeed = -1;
        while(newBuffIndex < 0){
            auxiliaryNeed ++;
            newBuffIndex += buffer.size();
        }
        if(auxiliaryNeed > auxiliaryIndex){
            auxiliaryBuffers.push_back(buffer);
            auxiliaryIndex++;
        }
        
        if(auxiliaryIndex != -1){
            tempOut[i] = auxiliaryBuffers[auxiliaryIndex][newBuffIndex];
        }else{
            tempOut[i] = buffer[newBuffIndex];
        }
    }
    parameters->get("Output").cast<vector<float>>() = tempOut;
}
