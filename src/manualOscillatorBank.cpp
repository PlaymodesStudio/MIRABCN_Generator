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
    bufferOverflow = 0;
}

void manualOscillatorBank::manualInputChanged(float &f){
//    while(bufferIndex >= buffer.size()){
//        buffer.push_back(f);
//    }
//    buffer[bufferIndex] = f;
//    ofLog() << buffer.size() << " " << bufferIndex;
}

void manualOscillatorBank::computeValues(float &f){
    if(oldPhasor > f) bufferOverflow++;
    oldPhasor = f;
    
    indexedBuffer.push_front(make_pair(f+bufferOverflow, manualInput));
    
    vector<float>   tempOut;
    tempOut.resize(indexs.size(), 0);
    
    int maxIndex = 0;
    for(int i = 0; i < indexs.size(); i++){
        float newBuffIndex = f+bufferOverflow - float(indexs[i]*delay);
        for(int j = 1; j < indexedBuffer.size(); j++){
            if(signbit(indexedBuffer[j-1].first - newBuffIndex) != signbit(indexedBuffer[j].first - newBuffIndex)){
                tempOut[i] = (indexedBuffer[j-1].second + indexedBuffer[j].second)*0.5;
                j = indexedBuffer.size();
            }
        }
//        if(newBuffIndex < buffer.size()){
//            tempOut[i] = buffer[newBuffIndex];
//        }
//        maxIndex = max(newBuffIndex, maxIndex);
    }
//    while(buffer.size() > maxIndex){
//        buffer.pop_back();
//    }
    
    parameters->get("Output").cast<vector<float>>() = tempOut;
}
