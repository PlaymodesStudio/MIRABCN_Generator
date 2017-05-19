//
//  oscillatorBank.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 09/01/2017.
//
//

#include "oscillatorBank.h"

oscillatorBank::oscillatorBank(int nOscillators, bool gui, int _bankId, ofPoint pos) : baseIndexer(nOscillators){
    bankId = _bankId;
    for(int i=0 ; i < nOscillators ; i++){
        oscillators.push_back(new baseOscillator(i));
        oscillators[i]->setIndexNormalized(indexs[i]);
        ofAddListener(oscillators[i]->resultGenerated, this, &oscillatorBank::oscillatorResult);
    }
    result.resize(nOscillators, 0);
    resultFilledChecker.resize(nOscillators, 0);
    
    parameters->setName("oscillatorBank " + ofToString(_bankId));
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
    
    if(gui)
        parametersControl::getInstance().createGuiFromParams(parameters, ofColor::red, pos);
    
    
    phasorIn.addListener(this, &oscillatorBank::newPhasorIn);
    phaseOffset_Param.addListener(this, &oscillatorBank::newPhaseOffsetParam);
    randomAdd_Param.addListener(this, &oscillatorBank::newRandomAddParam);
    scale_Param.addListener(this, &oscillatorBank::newScaleParam);
    offset_Param.addListener(this, &oscillatorBank::newOffsetParam);
    pow_Param.addListener(this, &oscillatorBank::newPowParam);
    quant_Param.addListener(this, &oscillatorBank::newQuantParam);
    amplitude_Param.addListener(this, &oscillatorBank::newAmplitudeParam);
    waveSelect_Param.addListener(this, &oscillatorBank::newWaveSelectParam);
    pwm_Param.addListener(this, &oscillatorBank::newPwmParam);
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
        if(waveSelect_Param == 6 || waveSelect_Param == 7){
            auto resultCopy = result;
            for(int i = 0 ; i<result.size() ; i++){
                int new_i = floor(i/((float)result.size()/(float)indexQuant_Param));
                result[i] = resultCopy[new_i];
            }
        }
        parameters->get("Oscillator Out").cast<vector<float>>() = result;
        fill(resultFilledChecker.begin(), resultFilledChecker.end(), 0);
        ofNotifyEvent(eventInGroup, bankId, this);
    }
}

void oscillatorBank::newPhasorIn(float &f){
    for(auto &oscillator : oscillators){
        oscillator->phasorIn = f;
    }
}

void oscillatorBank::newPowParam(int &i){
    for(auto &oscillator : oscillators){
        oscillator->pow_Param = i;
    }
}

void oscillatorBank::newPwmParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->pwm_Param = f;
    }
}

void oscillatorBank::newHoldTimeParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->holdTime_Param = f;
    }
}

void oscillatorBank::newPhaseOffsetParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->phaseOffset_Param = f;
    }
}

void oscillatorBank::newQuantParam(int &i){
    for(auto &oscillator : oscillators){
        oscillator->quant_Param = i;
    }
}

void oscillatorBank::newScaleParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->scale_Param = f;
    }
}

void oscillatorBank::newOffsetParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->offset_Param = f;
    }
}

void oscillatorBank::newRandomAddParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->randomAdd_Param = f;
    }
}

void oscillatorBank::newWaveSelectParam(int &i){
    for(auto &oscillator : oscillators){
        oscillator->waveSelect_Param = i;
    }
}

void oscillatorBank::newAmplitudeParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->amplitude_Param = f;
    }
}
