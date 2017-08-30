//
//  manualOscillatorBank.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 30/08/2017.
//
//

#include "manualOscillatorBank.h"
#include "parametersControl.h"

manualOscillatorBank::manualOscillatorBank(int nOscillators, int _bankId, ofPoint pos) : baseIndexer(nOscillators){
    
    parameters->setName("manualOscillatorBank " + ofToString(_bankId));
    parameters->add(phasorIn.set("Phasor In", 0, 0, 1));
    parameters->add(manualInput.set("Manual In", 0, 0, 1));
    parameters->add(delay.set("Delay", 0, 0, 1000));
    parameters->add(output.set("Output", {}));
    
    phasorIn.addListener(this, &manualOscillatorBank::computeValues);
    manualInput.addListener(this, &manualOscillatorBank::manualInputChanged);
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::blueSteel, pos);

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
//    bufferIndex++;
//    if(oldPhasor > f) bufferIndex = 0;
    
//    if(bufferIndex >= buffer.size()){
    buffer.push_front(manualInput);
//    }else{
//        buffer[bufferIndex] = manualInput;
//    }
    
    oldPhasor = f;
    vector<float>   tempOut;
    tempOut.resize(indexs.size(), 0);
    
    int maxIndex = 0;
    for(int i = 0; i < indexs.size(); i++){
        int newBuffIndex = int(indexs[i]*delay*indexs.size());
        if(newBuffIndex < buffer.size()){
            tempOut[i] = buffer[newBuffIndex];
        }
        maxIndex = max(newBuffIndex, maxIndex);
    }
    while(buffer.size() > maxIndex){
        buffer.pop_back();
    }
    
    parameters->get("Output").cast<vector<float>>() = tempOut;
}
