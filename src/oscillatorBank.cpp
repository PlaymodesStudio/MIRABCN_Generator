//
//  oscillatorBank.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 09/01/2017.
//
//

#include "oscillatorBank.h"

oscillatorBank::oscillatorBank(int nOscillators) : baseIndexer(nOscillators){
//    oscillators.resize(nOscillators, new baseOscillator());
    for(int i=0 ; i < nOscillators ; i++){
        oscillators.push_back(new baseOscillator(i));
        oscillators[i]->setIndexNormalized(indexs[i]);
        ofAddListener(oscillators[i]->resultGenerated, this, &oscillatorBank::oscillatorResult);
    }
    result.resize(nOscillators, 0);
    resultFilledChecker.resize(nOscillators, 0);
    
    parameters->setName("Oscillator Bank");
    parameters->add(phasorIn.set("Phasor In", 0, 0, 1));
    parameters->add(phaseOffset_Param.set("Phase Offset", 0, 0, 1));
    parameters->add(randomAdd_Param.set("Random Addition", 0, -.5, .5));
    parameters->add(scale_Param.set("Scale", 1, 0, 2));
    parameters->add(offset_Param.set("Offset", 0, -1, 1));
    parameters->add(pow_Param.set("Pow", 1, -40, 40));
    parameters->add(quant_Param.set("Quantization", 255, 1, 255));
    parameters->add(amplitude_Param.set("Fader", 1, 0, 1));
    ofParameterGroup waveDropDown;
    waveDropDown.setName("Wave Select");
    ofParameter<string> tempStrParam("Options", "sin-|-cos-|-tri-|-square-|-saw-|-inverted saw-|-rand1-|-rand2");
    waveDropDown.add(tempStrParam);
    waveDropDown.add(waveSelect_Param.set("Wave Select", 0, 0, 7));
    parameters->add(waveDropDown);
    parameters->add(pwm_Param.set("Square PWM", 0.5, 0, 1));
    parameters->add(oscillatorOut.set("Oscillator Out", {0}));
    
    parametersControl::getInstance().createGuiFromParams(parameters);
    phasorIn.addListener(this, &oscillatorBank::newPhasorIn);
}

void oscillatorBank::newIndexs(){
    for(int i=0 ; i < oscillators.size() ; i++){
        oscillators[i]->setIndexNormalized(indexs[i]);
    }
}

void oscillatorBank::oscillatorResult(pair<int, float> &oscInfo){
    result[oscInfo.first] = oscInfo.second;
    resultFilledChecker[oscInfo.first] = 1;
    
    if(resultFilledChecker.size() == accumulate(resultFilledChecker.begin(), resultFilledChecker.end(), 0)){
        parameters->get("Oscillator Out").cast<vector<float>>() = result;
        fill(resultFilledChecker.begin(), resultFilledChecker.end(), 0);
    }
}

void oscillatorBank::newPhasorIn(float &f){
    for(auto &oscillator : oscillators){
        oscillator->phasorIn = f;
    }
}
