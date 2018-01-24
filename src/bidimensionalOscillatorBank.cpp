//
//  bidimensionalOscillatorBank.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 15/11/2017.
//
//

#include "bidimensionalOscillatorBank.h"
#include "parametersControl.h"

bidimensionalOscillatorBank::bidimensionalOscillatorBank(int bankId, int xSize, int ySize, ofPoint pos){
    parameters = new ofParameterGroup();
    
    width = xSize;
    height = ySize;
    
    parameters->setName("bidimensionalOscillatorBank " + ofToString(bankId));
    
    indexers.push_back(new baseIndexer(xSize));
    indexers.push_back(new baseIndexer(ySize));
    
    int nOscillators = xSize * ySize;
    
    for(int i=0 ; i < nOscillators ; i++){
        oscillators.push_back(new baseOscillator(i));
//        oscillators[i]->setIndexNormalized(indexs[i]);
    }
    computeBidirectionalIndexs();
    result.resize(width, vector<float>(height));
    
    for(int i = 0; i < indexers.size(); i++){
        string dimensionStr = " ";
        int dimensionSize = 0;
        if(i == 0){
            dimensionStr = "X";
            dimensionSize = xSize;
        }else if(i == 1){
            dimensionStr = "Y";
            dimensionSize = ySize;
        }
        
        ofParameter<string> strlbl;
        parameters->add(strlbl.set(dimensionStr + " Dim_label", " "));
        
        parameters->add(indexers[i]->numWaves_Param.set("Num Waves " + dimensionStr, 1, 0, dimensionSize));
        parameters->add(indexers[i]->indexInvert_Param.set("Index Invert " + dimensionStr, 0, 0, 1));
        parameters->add(indexers[i]->symmetry_Param.set("Symmetry " + dimensionStr, 0, 0, 10));
        parameters->add(indexers[i]->indexRand_Param.set("Index Random " + dimensionStr, 0, 0, 1));
        parameters->add(indexers[i]->indexOffset_Param.set("Index Offset " + dimensionStr, 0, -dimensionSize/2, dimensionSize/2));
        parameters->add(indexers[i]->indexQuant_Param.set("Index Quantization " + dimensionStr, dimensionSize, 1, dimensionSize));
        parameters->add(indexers[i]->combination_Param.set("Index Combination " + dimensionStr, 0, 0, 1));
        parameters->add(indexers[i]->modulo_Param.set("Index Modulo " + dimensionStr, dimensionSize, 1, dimensionSize));
    }
    
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
    
    parameters->add(oscillatorOut.set("Oscillator Out", {{0}}));
    
    
    phasorIn.addListener(this, &bidimensionalOscillatorBank::newPhasorIn);
    phaseOffset_Param.addListener(this, &bidimensionalOscillatorBank::newPhaseOffsetParam);
    randomAdd_Param.addListener(this, &bidimensionalOscillatorBank::newRandomAddParam);
    scale_Param.addListener(this, &bidimensionalOscillatorBank::newScaleParam);
    offset_Param.addListener(this, &bidimensionalOscillatorBank::newOffsetParam);
    pow_Param.addListener(this, &bidimensionalOscillatorBank::newPowParam);
    quant_Param.addListener(this, &bidimensionalOscillatorBank::newQuantParam);
    amplitude_Param.addListener(this, &bidimensionalOscillatorBank::newAmplitudeParam);
    invert_Param.addListener(this, &bidimensionalOscillatorBank::newInvertParam);
    biPow_Param.addListener(this, &bidimensionalOscillatorBank::newBiPowParam);
    waveSelect_Param.addListener(this, &bidimensionalOscillatorBank::newWaveSelectParam);
    pulseWidth_Param.addListener(this, &bidimensionalOscillatorBank::newpulseWidthParam);
    skew_Param.addListener(this, &bidimensionalOscillatorBank::newSkewParam);
    
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::red, pos);
    
    ofAddListener(parameters->parameterChangedE(), this,  &bidimensionalOscillatorBank::parameterChanged);
}

void bidimensionalOscillatorBank::parameterChanged(ofAbstractParameter &p){
    
}

vector<vector<float>> bidimensionalOscillatorBank::computeBank(float phasor){
    int i_x = 0, i_y = 0;
    for(int i = 0; i < oscillators.size(); i++){
        i_y = floor(i/width);
        i_x = i - (i_y * width);
        result[i_x][i_y] = oscillators[i]->computeFunc(phasor);
    }
    return result;
}

void bidimensionalOscillatorBank::computeBidirectionalIndexs(){
    int lin_size = width * height;
    vector<float> tempOut(lin_size);
    if(width != 0 && height != 0){
        int width_i = 0, height_i = 0;
        for(int i = 0; i < lin_size; i++){
            height_i = floor(i/width);
            width_i = i - (height_i * width);
            oscillators[i]->setIndexNormalized(indexers[0]->getIndexs()[width_i]+ indexers[1]->getIndexs()[height_i]);
        }
    }
}

void bidimensionalOscillatorBank::newPhasorIn(float &f){
    if(indexers[0]->areNewIndexs() || indexers[1]->areNewIndexs()){
        computeBidirectionalIndexs();
    }
    computeBank(f);
    parameters->get("Oscillator Out").cast<vector<vector<float>>>() = result;
}

void bidimensionalOscillatorBank::newPowParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->pow_Param = f;
    }
}

void bidimensionalOscillatorBank::newpulseWidthParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->pulseWidth_Param = f;
    }
}

void bidimensionalOscillatorBank::newHoldTimeParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->holdTime_Param = f;
    }
}

void bidimensionalOscillatorBank::newPhaseOffsetParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->phaseOffset_Param = f;
    }
}

void bidimensionalOscillatorBank::newQuantParam(int &i){
    for(auto &oscillator : oscillators){
        oscillator->quant_Param = i;
    }
}

void bidimensionalOscillatorBank::newScaleParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->scale_Param = f;
    }
}

void bidimensionalOscillatorBank::newOffsetParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->offset_Param = f;
    }
}

void bidimensionalOscillatorBank::newRandomAddParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->randomAdd_Param = f;
    }
}

void bidimensionalOscillatorBank::newWaveSelectParam(int &i){
    for(auto &oscillator : oscillators){
        oscillator->waveSelect_Param = i;
    }
}

void bidimensionalOscillatorBank::newAmplitudeParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->amplitude_Param = f;
    }
}

void bidimensionalOscillatorBank::newInvertParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->invert_Param = f;
    }
}

void bidimensionalOscillatorBank::newSkewParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->skew_Param = f;
    }
}

void bidimensionalOscillatorBank::newBiPowParam(float &f){
    for(auto &oscillator : oscillators){
        oscillator->biPow_Param = f;
    }
}
