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


class phasorClass{
public:
    phasorClass(int index = 0, ofPoint pos = ofPoint(-1, -1));
    ~phasorClass();
    void setup(int index = 0);
    
    float getPhasor();
    void resetPhasor(bool &reset);
    
    void audioIn(float * input, int bufferSize, int nChannels);
    void loopChanged(bool &val);
    
    void nextFrame();
    
    ofParameterGroup* getParameterGroup(){return parameters;};
    
private:
    void initPhaseChanged(float &f);
    
    ofParameterGroup*    parameters;
    ofParameter<double>  phasor;
    ofParameter<double>  phasorMod;
    ofParameter<float>  bpm_Param;
    ofParameter<int>    beatsMult_Param;
    ofParameter<int>    beatsDiv_Param;
    ofParameter<float>  initPhase_Param;
    ofParameter<int>    quant_Param;
    ofParameter<float>  phasorMonitor;
    ofParameter<float>  minVal_Param;
    ofParameter<float>  maxVal_Param;
    ofParameter<bool>   loop_Param;
    ofParameter<bool>   bounce_Param;
    ofParameter<bool>   resetPhase_Param;
    ofParameter<bool>   offlineMode_Param;
};


#endif /* phasor_h */
