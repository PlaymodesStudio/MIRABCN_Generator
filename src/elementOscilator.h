//
//  elementOscilator.h
//  DGTL_Generator
//
//  Created by Eduard Frigola on 29/07/16.
//
//

#ifndef elementOscilator_h
#define elementOscilator_h

#include "ofMain.h"

enum oscTypes{
    sinOsc = 1,
    cosOsc = 2,
    triOsc = 3,
    squareOsc = 4,
    sawOsc = 5,
    sawInvOsc = 6,
    rand1Osc = 7,
    rand2Osc = 8
};

class elementOscilator{
public:
    elementOscilator();
    ~elementOscilator(){};
    
    void setup(int index = 0);
    
    void computeFunc(float *infoVec, float phasor, float modulation = -1);
    void computeMultiplyMod(float* value);
    
    void setIndexCount(int val){indexCount_Param = val;};
    
    ofParameterGroup getParameterGroup(){return parameters;};
    
private:
    //Parameters
    ofParameterGroup    parameters;
    ofParameter<float>  freq_Param; //Desphase Quantity
    ofParameter<int>    pow_Param; //Pow of the funcion, working on sin, cos....
    ofParameter<int>    indexCount_Param; //The max number you will get from index
    ofParameter<float>  pwm_Param;
    ofParameter<bool>   invert_Param;
    ofParameter<int>    symmetry_Param;
    ofParameter<float>  holdTime_Param; //The duration of the hold in percentage (0.5) --> 50% of the cycle is the phase in initPhase
    ofParameter<float>  phaseOffset_Param;
    ofParameter<int>    indexOffset_Param;
    ofParameter<int>    indexQuant_Param;
    ofParameter<float>  quant_Param;
    ofParameter<float>  scale_Param;
    ofParameter<float>  offset_Param;
    ofParameter<float>  comb_Param;
    ofParameter<int>    modulo_Param;
    ofParameter<float>  randomAdd_Param;
    ofParameter<int>    waveSelect_Param;
    ofParameter<int>    modulatorSelect_Param;
    
    vector<float>       infoVec_preMod;
    
    float               oldPhasor;
    int                 prevIndex;
};

#endif /* elementOscilator_h */
