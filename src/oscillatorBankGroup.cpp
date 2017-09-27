//
//  oscillatorBankGroup.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 13/01/2017.
//
//

#include "oscillatorBankGroup.h"

oscillatorBankGroup::oscillatorBankGroup(int oscillatorBankSize, int numOfOscillatorBanks, int id, ofPoint pos){
    
    //Last one is for preview, vector modulation does not affect this one
    for(int i=0 ; i < numOfOscillatorBanks + 1 ; i++){
        oscillatorBanks.push_back(new oscillatorBank(oscillatorBankSize, false, i));
    }

    result.resize(numOfOscillatorBanks, {0});
    resultFilledChecker.resize(numOfOscillatorBanks, 0);
    
    parameters = new ofParameterGroup;
    parameters->setName("oscillatorGroup " + ofToString(id));
    parameters->add(numWaves_Param.set("Num Waves", 1, 0, oscillatorBankSize));
    parameters->add(indexInvert_Param.set("Index Invert", 0, 0, 1));
    parameters->add(symmetry_Param.set("Symmetry", 0, 0, 10));
    parameters->add(indexRand_Param.set("Index Random", 0, 0, 1));
    parameters->add(indexOffset_Param.set("Index Offset", 0, -oscillatorBankSize/2, oscillatorBankSize/2));
    parameters->add(indexQuant_Param.set("Index Quantization", oscillatorBankSize, 1, oscillatorBankSize));
    parameters->add(combination_Param.set("Index Combination", 0, 0, 1));
    parameters->add(modulo_Param.set("Index Modulo", oscillatorBankSize, 1, oscillatorBankSize));
    
    parameters->add(phasorIn.set("Phasor In", 0, 0, 1));
    parameters->add(phaseOffset_Param.set("Phase Offset", 0, 0, 1));
    parameters->add(randomAdd_Param.set("Random Addition", 0, -.5, .5));
    parameters->add(scale_Param.set("Scale", 1, 0, 2));
    parameters->add(offset_Param.set("Offset", 0, -1, 1));
    parameters->add(pow_Param.set("Pow", 0, -40, 40));
    parameters->add(biPow_Param.set("Bi Pow", 0, -40, 40));
    parameters->add(quant_Param.set("Quantization", 255, 1, 255));
    parameters->add(pulseWidth_Param.set("Pulse Width", 1, 0, 1));
    parameters->add(skew_Param.set("Skew", 0, -1, 1));
    parameters->add(amplitude_Param.set("Fader", 1, 0, 1));
    parameters->add(invert_Param.set("Invert", 0, 0, 1));
    ofParameterGroup waveDropDown;
    waveDropDown.setName("Wave Select");
    ofParameter<string> tempStrParam("Options", "sin-|-cos-|-tri-|-square-|-saw-|-inverted saw-|-rand1-|-rand2");
    waveDropDown.add(tempStrParam);
    waveDropDown.add(waveSelect_Param.set("Wave Select", 0, 0, 7));
    parameters->add(waveDropDown);
    
    parameters->add(numWaves_vecParam.set("Num Waves Vector", {0}));
    parameters->add(indexInvert_vecParam.set("Index Invert Vector", {0}));
    parameters->add(symmetry_vecParam.set("Symmetry Vector", {0}));
    parameters->add(indexRand_vecParam.set("Index Random Vector", {0}));
    parameters->add(indexOffset_vecParam.set("Index Offset Vector", {0}));
    parameters->add(indexQuant_vecParam.set("Index Quantization Vector", {0}));
    parameters->add(combination_vecParam.set("Index Combination Vector", {0}));
    parameters->add(modulo_vecParam.set("Index Modulo Vector", {0}));
    
    parameters->add(phaseOffset_vecParam.set("Phase Offset Vector", {0}));
    parameters->add(randomAdd_vecParam.set("Random Addition Vector", {0}));
    parameters->add(scale_vecParam.set("Scale Vector", {0}));
    parameters->add(offset_vecParam.set("Offset Vector", {0}));
    parameters->add(pow_vecParam.set("Pow Vector", {0}));
    parameters->add(biPow_vecParam.set("Bi Pow Vector", {0}));
    parameters->add(quant_vecParam.set("Quantization Vector", {0}));
    parameters->add(pulseWidth_vecParam.set("Pulse Width Vector", {0}));
    parameters->add(skew_vecParam.set("Skew Vector", {0}));
    
    parameters->add(amplitude_vecParam.set("Fader Vector", {0}));
    parameters->add(invert_vecParam.set("Invert Vector", {0}));
    parameters->add(waveSelect_vecParam.set("Wave Select Vector", {0}));

    parameters->add(bankGroupOut.set("Main Out", {{0}}));
    parameters->add(previewOut.set("Preview Out", {0}));
    
    ofAddListener(parameters->parameterChangedE(), this, &oscillatorBankGroup::parameterChanged);
    phasorIn.addListener(this, &oscillatorBankGroup::phasorInListener);
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::blue, pos);
}

void oscillatorBankGroup::phasorInListener(float &phase){
    for (int i = 0; i < oscillatorBanks.size() - 1; i++){
        result[i] = oscillatorBanks[i]->computeBank(phase);
    }
    parameters->get("Main Out").cast<vector<vector<float>>>() = result;
    fill(resultFilledChecker.begin(), resultFilledChecker.end(), 0);
    
    parameters->get("Preview Out").cast<vector<float>>() = oscillatorBanks.back()->computeBank(phase);
}

void oscillatorBankGroup::parameterChanged(ofAbstractParameter &p){
    if(p.getName() == numWaves_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->numWaves_Param = numWaves_Param;
    }
    else if(p.getName() == indexInvert_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->indexInvert_Param = indexInvert_Param;
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
    
    else if(p.getName() == phaseOffset_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->phaseOffset_Param = phaseOffset_Param;
    }
    else if(p.getName() == pow_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->pow_Param = pow_Param;
    }
    else if(p.getName() == biPow_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
        oscBank->biPow_Param = biPow_Param;
    }
    else if(p.getName() == pulseWidth_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->pulseWidth_Param = pulseWidth_Param;
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
    else if(p.getName() == skew_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->skew_Param = skew_Param;
    }
    else if(p.getName() == waveSelect_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->waveSelect_Param = waveSelect_Param;
    }
    else if(p.getName() == amplitude_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->amplitude_Param = amplitude_Param;
    }
    else if(p.getName() == invert_Param.getName()){
        for(auto &oscBank : oscillatorBanks)
            oscBank->invert_Param = invert_Param;
    }
    
    
    
    //VECTORR
    if(p.getName() == numWaves_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->numWaves_Param = ofMap(numWaves_vecParam.get()[i], 0, 1, numWaves_Param.getMin(), numWaves_Param.getMax());
    }
    else if(p.getName() == indexInvert_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->indexInvert_Param =  ofMap(indexInvert_vecParam.get()[i], 0, 1, indexInvert_Param.getMin(), indexInvert_Param.getMax());
    }
    else if(p.getName() == symmetry_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->symmetry_Param = ofMap(symmetry_vecParam.get()[i], 0, 1, symmetry_Param.getMin(), symmetry_Param.getMax());
    }
    else if(p.getName() == indexRand_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->indexRand_Param = ofMap(indexRand_vecParam.get()[i], 0, 1, indexRand_Param.getMin(), indexRand_Param.getMax());
    }
    else if(p.getName() == indexOffset_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->indexOffset_Param = ofMap(indexOffset_vecParam.get()[i], 0, 1, indexOffset_Param.getMin(), indexOffset_Param.getMax());
    }
    else if(p.getName() == indexQuant_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->indexQuant_Param = ofMap(indexQuant_vecParam.get()[i], 0, 1, indexQuant_Param.getMin(), indexQuant_Param.getMax());
    }
    else if(p.getName() == combination_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->combination_Param = ofMap(combination_vecParam.get()[i], 0, 1, combination_Param.getMin(), combination_Param.getMax());
    }
    else if(p.getName() == modulo_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->modulo_Param = ofMap(modulo_vecParam.get()[i], 0, 1, modulo_Param.getMin(), modulo_Param.getMax());
    }

    else if(p.getName() == phaseOffset_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->phaseOffset_Param = ofMap(phaseOffset_vecParam.get()[i], 0, 1, phaseOffset_Param.getMin(), phaseOffset_Param.getMax());
    }
    else if(p.getName() == pow_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->pow_Param = ofMap(pow_vecParam.get()[i], 0, 1, pow_Param.getMin(), pow_Param.getMax());
    }
    else if(p.getName() == biPow_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->biPow_Param = ofMap(biPow_vecParam.get()[i], 0, 1, biPow_Param.getMin(), biPow_Param.getMax());
    }
    else if(p.getName() == pulseWidth_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->pulseWidth_Param = ofMap(pulseWidth_vecParam.get()[i], 0, 1, pulseWidth_Param.getMin(), pulseWidth_Param.getMax());
    }
    else if(p.getName() == holdTime_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->holdTime_Param = ofMap(holdTime_vecParam.get()[i], 0, 1, holdTime_Param.getMin(), holdTime_Param.getMax());
    }
    else if(p.getName() == quant_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->quant_Param = ofMap(quant_vecParam.get()[i], 0, 1, quant_Param.getMin(), quant_Param.getMax());
    }
    else if(p.getName() == scale_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->scale_Param = ofMap(scale_vecParam.get()[i], 0, 1, scale_Param.getMin(), scale_Param.getMax());
    }
    else if(p.getName() == offset_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->offset_Param = ofMap(offset_vecParam.get()[i], 0, 1, offset_Param.getMin(), offset_Param.getMax());
    }
    else if(p.getName() == randomAdd_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->randomAdd_Param = ofMap(randomAdd_vecParam.get()[i], 0, 1, randomAdd_Param.getMin(), randomAdd_Param.getMax());
    }
    else if(p.getName() == skew_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->skew_Param = ofMap(skew_vecParam.get()[i], 0, 1, skew_Param.getMin(), skew_Param.getMax());
    }
    else if(p.getName() == waveSelect_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->waveSelect_Param = ofMap(waveSelect_vecParam.get()[i], 0, 1, waveSelect_Param.getMin(), waveSelect_Param.getMax());
    }
    else if(p.getName() == amplitude_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->amplitude_Param = ofMap(amplitude_vecParam.get()[i], 0, 1, amplitude_Param.getMin(), amplitude_Param.getMax());
    }
    else if(p.getName() == invert_vecParam.getName()){
        for(int i = 0; i < oscillatorBanks.size() -1; i++)
            oscillatorBanks[i]->invert_Param = ofMap(invert_vecParam.get()[i], 0, 1, invert_Param.getMin(), invert_Param.getMax());
    }
}
