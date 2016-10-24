//
//  elementOscilator.cpp
//  DGTL_Generator
//
//  Created by Eduard Frigola on 29/07/16.
//
//

#include "elementOscilator.h"

elementOscilator::elementOscilator(){
    freq_Param = PI;
    indexCount_Param = 432;
    pow_Param = 1;
    invert_Param = false;
    symmetry_Param = 0;
}

void elementOscilator::setup(int index){
    parameters.setName("oscillator " + ofToString(index));
    parameters.add(freq_Param.set("Num Waves", 1, 0, indexCount_Param));
    parameters.add(phaseOffset_Param.set("Phase Offset", 0, 0, 1));
    parameters.add(invert_Param.set("Invert", false));
    parameters.add(symmetry_Param.set("Symmetry", 0, 0, 10));
    parameters.add(indexRand_Param.set("Index Random", 0, 0, 1));
    parameters.add(indexOffset_Param.set("Index Offset", 0, -indexCount_Param/2, indexCount_Param/2));
    parameters.add(indexQuant_Param.set("Index Quantization", indexCount_Param, 1, indexCount_Param));
    parameters.add(comb_Param.set("Combination", 0, 0, 1));
    parameters.add(modulo_Param.set("Modulo", indexCount_Param, 1, indexCount_Param));
    parameters.add(randomAdd_Param.set("Random Addition", 0, -.5, .5));
    parameters.add(scale_Param.set("Scale", 1, 0, 2));
    parameters.add(offset_Param.set("Offset", 0, -1, 1));
    parameters.add(pow_Param.set("Pow", 1, -40, 40));
    parameters.add(quant_Param.set("Quantization", 255, 1, 255));
    parameters.add(masterFader_Param.set("Master Fader", 1, 0, 1));
    ofParameterGroup waveDropDown;
    waveDropDown.setName("Wave Select");
    ofParameter<string> tempStrParam("Options", "sin-|-cos-|-tri-|-square-|-saw-|-inverted saw-|-rand1-|-rand2");
    waveDropDown.add(tempStrParam);
    waveDropDown.add(waveSelect_Param.set("Wave Select", 0, 0, 7));
    parameters.add(waveDropDown);
    parameters.add(pwm_Param.set("Square PWM", 0.5, 0, 1));
    
    vector<string> modulationOptions;
    for(int i=0 ; i<parameters.size(); i++){
        modulationOptions.push_back(parameters.get(i).getName());
    }
    string modulationOptionsString = "None-|-";
    for(auto opt : modulationOptions)
        modulationOptionsString += opt + "-|-";
    
    modulationOptionsString.erase(modulationOptionsString.end()-3, modulationOptionsString.end());

    ofParameterGroup modulationDropDown;
    modulationDropDown.setName("Modulation Select");
    ofParameter<string> temp2StrParam("Options", modulationOptionsString);
    modulationDropDown.add(temp2StrParam);
    modulationDropDown.add(modulatorSelect_Param.set("Modulation Select", 0, 0, 4));
    parameters.add(modulationDropDown);
    ofParameter<string> label("Modulator Min Max_label", " ");
    parameters.add(label);
    parameters.add(modulatorMin_Param.set("Modulator Min", 0, 0, 1));
    parameters.add(modulatorMax_Param.set("Modulator Max", 1, 0, 1));
    
    
    indexRand_Param.addListener(this, &elementOscilator::indexRandChanged);
    modulatorSelect_Param.addListener(this, &elementOscilator::modulatorChanged);
    
    infoVec_preMod.resize(indexCount_Param, 0);
    pastRandom.resize(indexCount_Param, 0);
    newRandom.resize(indexCount_Param, 0);
    indexRand.resize(indexCount_Param);
    for(int i = 0; i < indexRand.size(); i++)
        indexRand[i] = i;
    indexRand_Param_previous = 0;
}

void elementOscilator::computeFunc(float *infoVec, float phasor, float modulation){
    if(modulation != -1 && modulatorSelect_Param != 0){
        ofAbstractParameter &absParam = parameters.get(modulatorSelect_Param-1);
            if(absParam.type() == typeid(ofParameter<float>).name()){
                ofParameter<float> castedParam = parameters.getFloat(modulatorSelect_Param-1);
                float min_modulated = ofMap(modulatorMin_Param, 0, 1, castedParam.getMin(), castedParam.getMax(), true);
                float max_modulated = ofMap(modulatorMax_Param, 0, 1, castedParam.getMin(), castedParam.getMax(), true);
                float modulation_scaled = ofMap(modulation, 0, 1, min_modulated, max_modulated);
                castedParam.setWithoutEventNotifications(modulation_scaled);
            }
            else if(absParam.type() == typeid(ofParameter<int>).name()){
                ofParameter<int> castedParam = parameters.getInt(modulatorSelect_Param-1);
                int min_modulated = ofMap(modulatorMin_Param, 0, 1, castedParam.getMin(), castedParam.getMax(), true);
                int max_modulated = ofMap(modulatorMax_Param, 0, 1, castedParam.getMin(), castedParam.getMax(), true);
                int modulation_scaled = ofMap(modulation, 0, 1, min_modulated, max_modulated);
                castedParam.setWithoutEventNotifications(modulation_scaled);
            }
    }
    for (int i = 0; i < indexCount_Param ; i++){
        
//        if(modulatorSelect_Param == 3 && modulation != -1)
//            freq_Param = ofMap(modulation, 0, 1, freq_Param.getMin(), freq_Param.getMax());
//        
//        if(modulatorSelect_Param == 4 && modulation != -1)
//            comb_Param = ofMap(modulation, 0, 1, comb_Param.getMin(), comb_Param.getMax());
        
        int index = i;
        
        //get phasor to be w (radial freq)
        float w = (phasor*2*PI);// + (phaseOffset_Param*2*PI);
        
        //QUANTIZE
        int newNumOfPixels = indexQuant_Param;
        
        index = floor(index/((float)indexCount_Param/(float)newNumOfPixels));
        
        
        while(symmetry_Param > newNumOfPixels-1)
            symmetry_Param--;
        
        bool odd = false;
        if((int)((index)/(newNumOfPixels/(symmetry_Param+1)))%2 == 1 ) odd = true;
        
        index += indexOffset_Param;
        
        //SYMMETRY santi
        int veusSym = newNumOfPixels/(symmetry_Param+1);
        index = veusSym-abs((((int)(index/veusSym)%2) * veusSym)-(index%veusSym));
        
//        index = odd ? index-indexOffset_Param : index+indexOffset_Param;
        
        if(newNumOfPixels % 2 == 0)
            index += odd ? 1 : 0;

        
        //INVERSE
        //Fisrt we invert the index to simulate the wave goes from left to right, inverting indexes, if we want to invertit we don't do this calc
        if(invert_Param)
            index = ((float)indexCount_Param-(float)index);
        
        index += indexRand[index]*indexRand_Param;
        if(index > indexCount_Param)
            index - indexCount_Param;
        
        
        //cout<<index<<"-" ;
        
        //COMB
        index = abs(((index%2)*indexCount_Param*comb_Param)-index);
        
        //Modulo
        if(modulo_Param != modulo_Param.getMax())
            index %= modulo_Param;
        
        
        float k = (((float)index/(float)indexCount_Param) + phaseOffset_Param) * 2 * PI;
        
        
        
        //invert it?
        //k *= invert_Param;
        k *=  freq_Param * ((float)indexCount_Param/(float)indexQuant_Param); //Index Modifiers
        
//        if(modulatorSelect_Param == 1 && modulation != -1)
//            k+= (modulation*2*PI);
        
        float linPhase = fmod(w+k, 2*PI) / (2*PI);
        float val = 0;
        switch (static_cast<oscTypes>(waveSelect_Param.get()+1)) {
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
                if(phasor < oldPhasor){
                    if(index != prevIndex)
                        val = ofRandom(1);
                    else
                        val = infoVec_preMod[i-1];
                }
                else
                    val = infoVec_preMod[i];
                
                break;
            }
            case rand2Osc:
            {
                if(phasor < oldPhasor){
                    if(i == 0) pastRandom = newRandom;
                    if(index != prevIndex)
                        val = ofRandom(1);
                    else
                        val = infoVec_preMod[i-1];
                    
                    newRandom[i] = val;
                    val = pastRandom[i];
                }
                else
                    val = pastRandom[i]*(1-phasor) + newRandom[i]*phasor;
                
                break;
            }
            default:
                break;
        }
        
        infoVec_preMod[i] = val;
        
        computeMultiplyMod(&val);
        
//        if(modulatorSelect_Param == 2 && modulation != -1)
//            val *= modulation;
        
        if(i == indexCount_Param-1)
            oldPhasor = phasor;
        
        infoVec[i] = val;
        prevIndex = index;
    }
}

void elementOscilator::computeMultiplyMod(float *value){
    
    
    //random Add
    if(randomAdd_Param)
        *value += randomAdd_Param*ofRandom(1);
    
    *value = ofClamp(*value, 0, 1);
    
    //SCALE
    *value *= scale_Param;
    
    //OFFSETç
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
    
    *value *= masterFader_Param;
    
}


void elementOscilator::indexRandChanged(float &val){
    if(indexRand_Param_previous == 0)
        random_shuffle(indexRand.begin(), indexRand.end());
    indexRand_Param_previous = val;
}

void elementOscilator::modulatorChanged(int &index){
//    ofAbstractParameter &absParam = parameters.get(index);
//    if(absParam.type() == typeid(ofParameter<float>).name()){
//        ofParameter<float> castedParam = parameters.getFloat(index);
//        modulatorMin_Param.setMin(castedParam.getMin());
//        modulatorMin_Param.setMax(castedParam.getMax());
//        modulatorMax_Param.setMin(castedParam.getMin());
//        modulatorMax_Param.setMax(castedParam.getMax());
//    }
//    else if(absParam.type() == typeid(ofParameter<int>).name()){
//        ofParameter<int> castedParam = parameters.getInt(index);
//        modulatorMin_Param.setMin(castedParam.getMin());
//        modulatorMin_Param.setMax(castedParam.getMax());
//        modulatorMax_Param.setMin(castedParam.getMin());
//        modulatorMax_Param.setMax(castedParam.getMax());
//    }
}



