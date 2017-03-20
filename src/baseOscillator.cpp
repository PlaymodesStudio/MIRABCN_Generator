//
//  baseOscillator.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 10/01/2017.
//
//

#include "baseOscillator.h"

baseOscillator::baseOscillator(int id, bool gui, ofPoint pos){
    oscId = id;
    parameters = new ofParameterGroup();
    parameters->setName("oscillator " + ofToString(index));
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
    parameters->add(output.set("Output", 0, 0, 1));
    
    phasorIn.addListener(this, &baseOscillator::computeFunc);
    
    if(gui)
        parametersControl::getInstance().createGuiFromParams(parameters, ofColor::orange, pos);
    
    oldPhasor = 0;
    oldValuePreMod = 0;
    indexNormalized = 0;
}

void baseOscillator::computeFunc(float &phasor){
    //get phasor to be w (radial freq)
    float w = (phasor*2*PI);
    
    float k = (indexNormalized + phaseOffset_Param) * 2 * PI;
//
//    //invert it?
//    k *=  freq_Param * ((float)indexCount_Param/(float)indexQuant_Param); //Index Modifiers
    
    
    float linPhase = fmod(w+k, 2*PI) / (2*PI);
    float val = 0;
    switch (static_cast<oscTypes>(waveSelect_Param.get()+1)){
        case sinOsc:
        {
            val = sin(w+k);
            val = ofMap(val, -1, 1, 0, 1);
            break;
            
        }
        case cosOsc:
        {
            val = cos(w+k);
            val = ofMap(val, -1, 1, 0, 1);
            break;
        }
        case triOsc:
        {
            val = 1-(fabs((linPhase * (-2)) + 1));
            break;
        }
        case squareOsc:
        {
            val = (linPhase > pwm_Param) ? 1 : 0;
            break;
        }
        case sawOsc:
        {
            val = 1-linPhase;
            break;
        }
        case sawInvOsc:
        {
            val = linPhase;
            break;
        }
        case rand1Osc:
        {
//            srand(linPhase*100000000);
//            val = ofRandomuf();
            if(phasor < oldPhasor){
//                if(index != prevIndex)
                    val = ofRandom(1);
//                else
//                    val = infoVec_preMod[i-1];
            }
            else
                val = oldValuePreMod;
            
            break;
        }
        case rand2Osc:
        {
            if(phasor < oldPhasor){
                pastRandom = newRandom;
//                if(index != prevIndex)
                    newRandom = ofRandom(1);
//                else
//                    newRandom[i] = newRandom[i-1];
//                
                val = pastRandom;
            }
            else
                val = pastRandom*(1-phasor) + newRandom*phasor;
            
            break;
        }
        default:
            break;
    }
    
    oldValuePreMod = val;
    
    computeMultiplyMod(&val);
    
    oldPhasor = phasor;
    
    parameters->getFloat("Output") = val;
    pair<int, float> toSend;
    toSend.first = oscId;
    toSend.second = val;
    ofNotifyEvent(resultGenerated, toSend, this);
}

void baseOscillator::computeMultiplyMod(float *value){
    
    
    //random Add
    if(randomAdd_Param)
        *value += randomAdd_Param*ofRandom(1);
    
    *value = ofClamp(*value, 0, 1);
    
    //SCALE
    *value *= scale_Param;
    
    //OFFSET√ß
    *value += offset_Param;
    
    *value = ofClamp(*value, 0, 1);
    
    //pow
    if(pow_Param)
        *value = (pow_Param < 0) ? pow(*value, 1/(float)(-pow_Param)) : pow(*value, pow_Param);
    
    *value = ofClamp(*value, 0, 1);
    
    //Quantization
    if(quant_Param < 255)
        *value = (1/(float)quant_Param)*round(*value*quant_Param);
    
    *value = ofClamp(*value, 0, 1);
    
    *value *= amplitude_Param;
}

