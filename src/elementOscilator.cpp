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
    parameters.add(freq_Param.set("n Waves", 1, 0, indexCount_Param/20));
    parameters.add(phaseOffset_Param.set("Phase offset", 0, 0, 1));
    parameters.add(invert_Param.set("Invert", false));
    parameters.add(symmetry_Param.set("Symmetry", 0, 0, 10));
    parameters.add(indexOffset_Param.set("Index Offset", 0, -indexCount_Param/2, indexCount_Param/2));
    parameters.add(indexQuant_Param.set("Index Quantization", 1, 1, indexCount_Param));
    parameters.add(comb_Param.set("Combination", 0, 0, 1));
    parameters.add(modulo_Param.set("Modulo", indexCount_Param, 1, indexCount_Param));
    parameters.add(randomAdd_Param.set("Random addition", 0, -.5, .5));
    parameters.add(quant_Param.set("Quantization", 0, 0, 1));
    parameters.add(scale_Param.set("Scale", 1, .5, 1.5));
    parameters.add(offset_Param.set("Offset", 0, -.5, .5));
    parameters.add(pow_Param.set("Pow", 1, -40, 40));
    parameters.add(pwm_Param.set("Square PWM", 0.5, 0, 1));
    waveDropDown.setName("Wave Select");
    ofParameter<string> tempStrParam("Options", "sin-|-cos-|-tri-|-square-|-saw-|-inverted saw-|-rand1-|-rand2");
    waveDropDown.add(tempStrParam);
    waveDropDown.add(waveSelect_Param.set("Wave Select", 0, 0, 7));
    parameters.add(waveDropDown);
    //parameters.add(waveSelect_Param.set("Wave Select", 0, 0, 7));
    
    infoVec_preMod.resize(indexCount_Param);
}

void elementOscilator::computeFunc(float *infoVec, float phasor, float phase){
    for (int i = 0; i < indexCount_Param ; i++){
        
        int index = i;
        
        //get phasor to be w (radial freq)
        float w = (phasor*2*PI);// + (phaseOffset_Param*2*PI);
        
        //QUANTIZE
        index = ceil(index/indexQuant_Param);
        
        int newNumOfPixels = indexCount_Param/indexQuant_Param;
        
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
        
        
        
        
        //cout<<index<<"-" ;
        
        //COMB
        index = abs(((index%2)*indexCount_Param*comb_Param)-index);
        
        //Modulo
        if(modulo_Param != modulo_Param.getMax())
            index %= modulo_Param;
        
        
        float k = (((float)index/(float)indexCount_Param) + phaseOffset_Param) * 2 * PI;
        
        
        
        //invert it?
        //k *= invert_Param;
        k *=  freq_Param * indexQuant_Param; //Index Modifiers
        
        k+= (phase*2*PI);
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
                val = ofNoise(w+((float)index/(float)indexCount_Param));
                val = ofMap(val, 0.25, 0.75, 0, 1, true);
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
}

void elementOscilator::computeFunc(float *infoVec, float phasor){
    computeFunc(infoVec, phasor, 0);
}

void elementOscilator::computeMultiplyMod(float *value){
  
    //SCALE
    *value *= scale_Param;
    
    //OFFSETç
    *value += offset_Param;
    
    //random Add
    if(randomAdd_Param)
        *value += randomAdd_Param*ofRandom(1);
    
    *value = ofClamp(*value, 0, 1);
    
    //pow
    if(pow_Param)
        *value = (pow_Param < 0) ? pow(*value, 1/(float)(-pow_Param)) : pow(*value, pow_Param);
    
    *value = ofClamp(*value, 0, 1);
    
    //Quantization
    if(quant_Param)
        *value = quant_Param*round(*value/quant_Param);
    
    *value = ofClamp(*value, 0, 1);
    
}





