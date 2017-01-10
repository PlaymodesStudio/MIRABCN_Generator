//
//  oscillatorBank.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 09/01/2017.
//
//

#ifndef oscillatorBank_h
#define oscillatorBank_h

#include "baseIndexer.h"
#include "baseOscillator.h"
#include "ofExpandableParameter.h"

//This class will contain a set of oscillators and has to inherit the indexer class (or bank class)
class oscillatorBank : public baseIndexer{
public:
    oscillatorBank(int nOscillators);
    ~oscillatorBank(){};
private:
    virtual void newIndexs();
    void oscillatorResult(pair<int, float> &oscInfo);
    void newPhasorIn(float &f);
    
    vector<baseOscillator*> oscillators;
    
    ofExpandableParameter<float>    phasorIn;
    ofExpandableParameter<int>      pow_Param; //Pow of the funcion, working on sin, cos....
    ofExpandableParameter<float>    pwm_Param;
    ofExpandableParameter<float>    holdTime_Param; //The duration of the hold in percentage (0.5) --> 50% of the cycle is the phase in initPhase
    ofExpandableParameter<float>    phaseOffset_Param;
    ofExpandableParameter<int>      quant_Param;
    ofExpandableParameter<float>    scale_Param;
    ofExpandableParameter<float>    offset_Param;
    ofExpandableParameter<float>    randomAdd_Param;
    ofExpandableParameter<int>      waveSelect_Param;
    ofExpandableParameter<float>    amplitude_Param;
    ofParameter<vector<float>>      oscillatorOut;
    
    vector<float> result;
    vector<float> resultFilledChecker;

};

#endif /* oscillatorBank_h */
