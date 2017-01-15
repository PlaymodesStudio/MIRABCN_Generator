//
//  oscillatorBankGroup.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 13/01/2017.
//
//

#include "oscillatorBankGroup.h"

oscillatorBankGroup::oscillatorBankGroup(int oscillatorBankSize, int numOfOscillatorBanks){
    
    for(int i=0 ; i < numOfOscillatorBanks + 1 ; i++){
        oscillatorBanks.push_back(new oscillatorBank(oscillatorBankSize, false, i));
        ofAddListener(oscillatorBanks[i]->eventInGroup, this, &oscillatorBankGroup::oscillatorBankResult);
    }

    result.resize(numOfOscillatorBanks, {0});
    resultFilledChecker.resize(numOfOscillatorBanks, 0);
    
    parameters = new ofParameterGroup;
    parameters->setName("oscillator " + ofToString(index));
    parameters->add(numWaves_Param.set("Num Waves", 1, 0, oscillatorBankSize));
    parameters->add(invert_Param.set("Invert", false));
    parameters->add(symmetry_Param.set("Symmetry", 0, 0, 10));
    parameters->add(indexRand_Param.set("Index Random", 0, 0, 1));
    parameters->add(indexOffset_Param.set("Index Offset", 0, -oscillatorBankSize/2, oscillatorBankSize/2));
    parameters->add(indexQuant_Param.set("Index Quantization", oscillatorBankSize, 1, oscillatorBankSize));
    parameters->add(combination_Param.set("Combination", 0, 0, 1));
    parameters->add(modulo_Param.set("Modulo", oscillatorBankSize, 1, oscillatorBankSize));
    
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
    
    parameters->add(numWaves_vecParam.set("Num Waves Vector", {0}));
    parameters->add(invert_vecParam.set("Invert Vector", {0}));
    parameters->add(symmetry_vecParam.set("Symmetry Vector", {0}));
    parameters->add(indexRand_vecParam.set("Index Random Vector", {0}));
    parameters->add(indexOffset_vecParam.set("Index Offset Vector", {0}));
    parameters->add(indexQuant_vecParam.set("Index Quantization Vector", {0}));
    parameters->add(combination_vecParam.set("Combination Vector", {0}));
    parameters->add(modulo_vecParam.set("Modulo Vector", {0}));
    
    parameters->add(phaseOffset_vecParam.set("Phase Offset Vector", {0}));
    parameters->add(randomAdd_vecParam.set("Random Addition Vector", {0}));
    parameters->add(scale_vecParam.set("Scale Vector", {0}));
    parameters->add(offset_vecParam.set("Offset Vector", {0}));
    parameters->add(pow_vecParam.set("Pow Vector", {0}));
    parameters->add(quant_vecParam.set("Quantization Vector", {0}));
    parameters->add(amplitude_vecParam.set("Fader Vector", {0}));
    parameters->add(waveSelect_vecParam.set("Wave Select Vector", {0}));
    parameters->add(pwm_vecParam.set("Square PWM Vector", {0}));
    
    parameters->add(bankGroupOut.set("Main Out", {{0}}));
    parameters->add(previewOut.set("Preview Out", {0}));
    
    ofAddListener(parameters->parameterChangedE(), this, &oscillatorBankGroup::parameterChanged);
    phasorIn.addListener(this, &oscillatorBankGroup::phasorInListener);
    
    parametersControl::getInstance().createGuiFromParams(parameters);
}

void oscillatorBankGroup::phasorInListener(float &phase){
    for(auto &oscBank : oscillatorBanks)
        oscBank->phasorIn = phase;
}

void oscillatorBankGroup::parameterChanged(ofAbstractParameter &p){
    if(p.getName() == numWaves_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->numWaves_Param = numWaves_Param;
    }
    else if(p.getName() == invert_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->invert_Param = invert_Param;
    }
    else if(p.getName() == symmetry_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->symmetry_Param = symmetry_Param;
    }
    else if(p.getName() == indexRand_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->indexRand_Param = indexRand_Param;
    }
    else if(p.getName() == indexOffset_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->indexOffset_Param = indexOffset_Param;
    }
    else if(p.getName() == indexQuant_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->indexQuant_Param = indexQuant_Param;
    }
    else if(p.getName() == combination_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->combination_Param = combination_Param;
    }
    else if(p.getName() == modulo_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->modulo_Param = modulo_Param;
    }
    
    else if(p.getName() == pow_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->pow_Param = pow_Param;
    }
    else if(p.getName() == pwm_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->pwm_Param = pwm_Param;
    }
    else if(p.getName() == holdTime_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->holdTime_Param = holdTime_Param;
    }
    else if(p.getName() == quant_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->quant_Param = quant_Param;
    }
    else if(p.getName() == scale_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->scale_Param = scale_Param;
    }
    else if(p.getName() == offset_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->offset_Param = offset_Param;
    }
    else if(p.getName() == randomAdd_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->randomAdd_Param = randomAdd_Param;
    }
    else if(p.getName() == waveSelect_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->waveSelect_Param = waveSelect_Param;
    }
    else if(p.getName() == amplitude_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->amplitude_Param = amplitude_Param;
    }
    
    
    
    //VECTORR
    if(p.getName() == numWaves_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size(); i++)
            oscillatorBanks[i]->numWaves_Param = numWaves_vecParam.get()[i];
    }
    else if(p.getName() == invert_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size(); i++)
            oscillatorBanks[i]->invert_Param = invert_vecParam.get()[i];
    }
    else if(p.getName() == symmetry_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size(); i++)
            oscillatorBanks[i]->symmetry_Param = symmetry_vecParam.get()[i];
    }
    else if(p.getName() == indexRand_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size(); i++)
            oscillatorBanks[i]->indexRand_Param = indexRand_vecParam.get()[i];
    }
    else if(p.getName() == indexOffset_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size(); i++)
            oscillatorBanks[i]->indexOffset_Param = indexOffset_vecParam.get()[i];
    }
    else if(p.getName() == indexQuant_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size(); i++)
            oscillatorBanks[i]->indexQuant_Param = indexQuant_vecParam.get()[i];
    }
    else if(p.getName() == combination_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size(); i++)
            oscillatorBanks[i]->combination_Param = combination_vecParam.get()[i];
    }
    else if(p.getName() == modulo_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size(); i++)
            oscillatorBanks[i]->modulo_Param = modulo_vecParam.get()[i];
    }

    else if(p.getName() == pow_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size(); i++)
            oscillatorBanks[i]->pow_Param = pow_vecParam.get()[i];
    }
    else if(p.getName() == pwm_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size(); i++)
            oscillatorBanks[i]->pwm_Param = pwm_vecParam.get()[i];
    }
    else if(p.getName() == holdTime_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size(); i++)
            oscillatorBanks[i]->holdTime_Param = holdTime_vecParam.get()[i];
    }
    else if(p.getName() == quant_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size(); i++)
            oscillatorBanks[i]->quant_Param = quant_vecParam.get()[i];
    }
    else if(p.getName() == scale_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size(); i++)
            oscillatorBanks[i]->scale_Param = scale_vecParam.get()[i];
    }
    else if(p.getName() == offset_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size(); i++)
            oscillatorBanks[i]->offset_Param = offset_vecParam.get()[i];
    }
    else if(p.getName() == randomAdd_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size(); i++)
            oscillatorBanks[i]->randomAdd_Param = randomAdd_vecParam.get()[i];
    }
    else if(p.getName() == waveSelect_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size(); i++)
            oscillatorBanks[i]->waveSelect_Param = waveSelect_vecParam.get()[i];
    }
    else if(p.getName() == amplitude_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size(); i++)
            oscillatorBanks[i]->amplitude_Param = amplitude_vecParam.get()[i];
    }
}

void oscillatorBankGroup::oscillatorBankResult(int &oscBankInfo){
    if (oscBankInfo < result.size()){
        result[oscBankInfo] = oscillatorBanks[oscBankInfo]->oscillatorOut;
        resultFilledChecker[oscBankInfo] = 1;
        
        if(resultFilledChecker.size() == accumulate(resultFilledChecker.begin(), resultFilledChecker.end(), 0)){
            parameters->get("Main Out").cast<vector<vector<float>>>() = result;
            fill(resultFilledChecker.begin(), resultFilledChecker.end(), 0);
        }
    }else{
        parameters->get("Preview Out").cast<vector<float>>() = oscillatorBanks[oscBankInfo]->oscillatorOut;
    }
}
