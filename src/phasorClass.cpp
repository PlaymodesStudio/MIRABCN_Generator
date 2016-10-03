//
//  phasorClass.cpp
//  DGTL_Generator
//
//  Created by Eduard Frigola on 28/07/16.
//
//

#include "phasorClass.h"


phasorClass::phasorClass()
{

}

void phasorClass::setup(int index){
    parameters.setName("phasor " + ofToString(index));
    parameters.add(bpm_Param.set("BPM", 120, 30, 180));
    parameters.add(beatsDiv_Param.set("Beats Div", 1, 1, 12));
    parameters.add(beatsMult_Param.set("Beats Mult", 1, 1, 12));
    parameters.add(quant_Param.set("Quantization", 40, 1, 40));
    parameters.add(initPhase_Param.set("Initial phase", 0, 0, 1));
    parameters.add(resetPhase_Param.set("Reset Phase", false));
    parameters.add(loop_Param.set("Loop", true));
    parameters.add(bounce_Param.set("Bounce", false));
    parameters.add(phasorMonitor.set("Phasor Monitor", 0));
    
    resetPhase_Param.addListener(this, &phasorClass::resetPhasor);
}


float phasorClass::getPhasor(){
    phasorMonitor = phasorMod;
    return phasorMod;
}

void phasorClass::resetPhasor(bool &reset){
    phasor = 0;
    resetPhase_Param = false;
}

void phasorClass::audioIn(float * input, int bufferSize, int nChannels){
    //tue phasor that goes from 0 to 1 at desired frequency
    float freq = (float)bpm_Param/(float)60;
    freq = freq * (float)beatsMult_Param;
    freq = (float)freq / (float)beatsDiv_Param;
    
    float increment = (1.0f/(float)(((float)44100/(float)512)/(float)freq));
    
    // We want to use half speed with bounce param,
    // becouse we will make a triangle wave and it
    // will go and return with the same period
    // it we don't change it
    if ( phasor < 1)
        phasor = bounce_Param ? phasor + increment/2 : phasor + increment;
    else if ( phasor >= 1.0 && loop_Param) phasor -= 1.0;
    
    //Assign a copy of the phasor to make some modifications
    phasorMod = phasor;
    
    //We make a triangle Wave
    if(bounce_Param)
        phasorMod = 1-(fabs((phasor * (-2))+ 1));
    
    //take the initPhase_Param as a phase offset param
    phasorMod += initPhase_Param;
    if(phasorMod >= 1.0)
        phasorMod -= 1.0;
    
    
    //Quantization -- only get the values we are interested
    if(quant_Param != 40){
        phasorMod = (int)(phasorMod*quant_Param);
        phasorMod /= quant_Param;
    }
}

