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
    
    ofParameter<float>    phasorIn;
    ofParameter<int>      pow_Param; //Pow of the funcion, working on sin, cos....
    ofParameter<float>    pwm_Param;
    ofParameter<float>    holdTime_Param; //The duration of the hold in percentage (0.5) --> 50% of the cycle is the phase in initPhase
    ofParameter<float>    phaseOffset_Param;
    ofParameter<int>      quant_Param;
    ofParameter<float>    scale_Param;
    ofParameter<float>    offset_Param;
    ofParameter<float>    randomAdd_Param;
    ofParameter<int>      waveSelect_Param;
    ofParameter<float>    amplitude_Param;
    ofParameter<vector<float>>      oscillatorOut;
    
    vector<float> result;
    vector<int> resultFilledChecker;

};

#endif /* oscillatorBank_h */
