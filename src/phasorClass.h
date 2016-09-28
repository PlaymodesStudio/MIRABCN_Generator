//
//  phasorClass.h
//  DGTL_Generator
//
//  Created by Eduard Frigola on 28/07/16.
//
//

#ifndef phasorClass_h
#define phasorClass_h

#pragma once

#include "ofMain.h"

class phasorClass: public ofBaseSoundInput{
public:
    phasorClass();
    ~phasorClass(){};
    void setup();
    
    float getPhasor();
    void resetPhasor(bool &reset);
    
    void audioIn(float * input, int bufferSize, int nChannels);
    
    ofParameterGroup getParameterGroup(){return parameters;};
    
private:
    ofParameterGroup    parameters;
    ofParameter<float>  phasor;
    ofParameter<float>  phasorMod;
    ofParameter<float>  bpm_Param;
    ofParameter<int>    beatsMult_Param;
    ofParameter<int>    beatsDiv_Param;
    ofParameter<float>  initPhase_Param;
    ofParameter<int>    quant_Param;
    ofParameter<bool>   loop_Param;
    ofParameter<bool>   bounce_Param;
    ofParameter<bool>   resetPhase_Param;
    
    ofSoundStream soundStream;
};


#endif /* phasor_h */
