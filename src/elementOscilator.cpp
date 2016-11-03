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
    previousModulation1 = 0;
    previousModulation2 = 0;
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

    ofParameterGroup modulationDropDown1;
    modulationDropDown1.setName("Modulation1 Select");
    ofParameter<string> temp2StrParam("Options", modulationOptionsString);
    modulationDropDown1.add(temp2StrParam);
    modulationDropDown1.add(modulatorSelect_Param1.set("Modulation1 Select", 0, 0, 4));
    parameters.add(modulationDropDown1);
    ofParameter<string> label1("MODULATOR1 MIIN MAX_label", " ");
    parameters.add(label1);
    parameters.add(modulatorMin_Param1.set("Modulator1 Min", 0, 0, 1));
    parameters.add(modulatorMax_Param1.set("Modulator1 Max", 1, 0, 1));
    
    ofParameterGroup modulationDropDown2;
    modulationDropDown2.setName("Modulation2 Select");
    modulationDropDown2.add(temp2StrParam);
    modulationDropDown2.add(modulatorSelect_Param2.set("Modulation2 Select", 0, 0, 4));
    parameters.add(modulationDropDown2);
    ofParameter<string> label2("MODULATOR2 MIIN MAX_label", " ");
    parameters.add(label2);
    parameters.add(modulatorMin_Param2.set("Modulator2 Min", 0, 0, 1));
    parameters.add(modulatorMax_Param2.set("Modulator2 Max", 1, 0, 1));
    
    indexRand_Param.addListener(this, &elementOscilator::indexRandChanged);
    
    infoVec_preMod.resize(indexCount_Param, 0);
    pastRandom.resize(indexCount_Param, 0);
    newRandom.resize(indexCount_Param, 0);
    indexRand.resize(indexCount_Param);
    for(int i = 0; i < indexRand.size(); i++)
        indexRand[i] = i-(indexRand.size()/2);
    indexRand_Param_previous = 0;
}

void elementOscilator::computeFunc(float *infoVec, float phasor, float modulation){
    //TODO: Directily modulate the parameters it's not the best way, becouse we loose the value, in gui and for the other iterations of the same func.
    // a Way could be to duplicate the values (don't think is the best way.
    // another way could be to copy the value that it's modulated before it's modulated, and then return that value to the parameter once exit the function.
    if(modulation != -1 && modulatorSelect_Param1 != 0){
        ofAbstractParameter &absParam = parameters.get(modulatorSelect_Param1-1);
            if(absParam.type() == typeid(ofParameter<float>).name()){
                ofParameter<float> castedParam = parameters.getFloat(modulatorSelect_Param1-1);
                float min_modulated = ofMap(modulatorMin_Param1, 0, 1, castedParam.getMin(), castedParam.getMax(), true);
                float max_modulated = ofMap(modulatorMax_Param1, 0, 1, castedParam.getMin(), castedParam.getMax(), true);
                float modulation_scaled = ofMap(modulation, 0, 1, min_modulated, max_modulated);
                modulatedParameter_noModValue1 = castedParam;
                castedParam.setWithoutEventNotifications(modulation_scaled);
            }
            else if(absParam.type() == typeid(ofParameter<int>).name()){
                ofParameter<int> castedParam = parameters.getInt(modulatorSelect_Param1-1);
                int min_modulated = ofMap(modulatorMin_Param1, 0, 1, castedParam.getMin(), castedParam.getMax(), true);
                int max_modulated = ofMap(modulatorMax_Param1, 0, 1, castedParam.getMin(), castedParam.getMax(), true);
                int modulation_scaled = ofMap(modulation, 0, 1, min_modulated, max_modulated);
                modulatedParameter_noModValue1 = castedParam;
                castedParam.setWithoutEventNotifications(modulation_scaled);
            }
    }
    
    if(modulation != -1 && modulatorSelect_Param2 != 0){
        ofAbstractParameter &absParam = parameters.get(modulatorSelect_Param2-1);
        if(absParam.type() == typeid(ofParameter<float>).name()){
            ofParameter<float> castedParam = parameters.getFloat(modulatorSelect_Param2-1);
            float min_modulated = ofMap(modulatorMin_Param2, 0, 1, castedParam.getMin(), castedParam.getMax(), true);
            float max_modulated = ofMap(modulatorMax_Param2, 0, 1, castedParam.getMin(), castedParam.getMax(), true);
            float modulation_scaled = ofMap(modulation, 0, 1, min_modulated, max_modulated);
            modulatedParameter_noModValue2 = castedParam;
            castedParam.setWithoutEventNotifications(modulation_scaled);
        }
        else if(absParam.type() == typeid(ofParameter<int>).name()){
            ofParameter<int> castedParam = parameters.getInt(modulatorSelect_Param2-1);
            int min_modulated = ofMap(modulatorMin_Param2, 0, 1, castedParam.getMin(), castedParam.getMax(), true);
            int max_modulated = ofMap(modulatorMax_Param2, 0, 1, castedParam.getMin(), castedParam.getMax(), true);
            int modulation_scaled = ofMap(modulation, 0, 1, min_modulated, max_modulated);
            modulatedParameter_noModValue2 = castedParam;
            castedParam.setWithoutEventNotifications(modulation_scaled);
        }
    }
    
    prevIndex = -1;
    
    for (int i = 0; i < indexCount_Param ; i++){
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
        
        
        if(newNumOfPixels % 2 == 0)
            index += odd ? 1 : 0;

        
        //INVERSE
        //Fisrt we invert the index to simulate the wave goes from left to right, inverting indexes, if we want to invertit we don't do this calc
        if(invert_Param)
            index = ((float)indexCount_Param-(float)index);
        
        index += indexRand[index-1]*indexRand_Param;
        if(index > indexCount_Param)
            index - indexCount_Param;
        
        
        //COMB
        index = abs(((index%2)*indexCount_Param*comb_Param)-index);
        
        //Modulo
        if(modulo_Param != modulo_Param.getMax())
            index %= modulo_Param;
        
        
        float k = (((float)index/(float)indexCount_Param) + phaseOffset_Param) * 2 * PI;
        
        
        
        //invert it?
        k *=  freq_Param * ((float)indexCount_Param/(float)indexQuant_Param); //Index Modifiers
        
        
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
                        newRandom[i] = ofRandom(1);
                    else
                        newRandom[i] = newRandom[i-1];
                    
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
        
        if(i == indexCount_Param-1)
            oldPhasor = phasor;
        
        infoVec[i] = val;
        prevIndex = index;
    }
    
    if(modulation != -1 && modulatorSelect_Param1 != 0){
        ofAbstractParameter &absParam = parameters.get(modulatorSelect_Param1-1);
        if(absParam.type() == typeid(ofParameter<float>).name()){
            ofParameter<float> castedParam = parameters.getFloat(modulatorSelect_Param1-1);
            castedParam.setWithoutEventNotifications(modulatedParameter_noModValue1);
        }
        else if(absParam.type() == typeid(ofParameter<int>).name()){
            ofParameter<int> castedParam = parameters.getInt(modulatorSelect_Param1-1);
            castedParam.setWithoutEventNotifications(modulatedParameter_noModValue1);
        }
    }
    if(modulation != -1 && modulatorSelect_Param2 != 0){
        ofAbstractParameter &absParam = parameters.get(modulatorSelect_Param2-1);
        if(absParam.type() == typeid(ofParameter<float>).name()){
            ofParameter<float> castedParam = parameters.getFloat(modulatorSelect_Param2-1);
            castedParam.setWithoutEventNotifications(modulatedParameter_noModValue2);
        }
        else if(absParam.type() == typeid(ofParameter<int>).name()){
            ofParameter<int> castedParam = parameters.getInt(modulatorSelect_Param2-1);
            castedParam.setWithoutEventNotifications(modulatedParameter_noModValue2);
        }
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



