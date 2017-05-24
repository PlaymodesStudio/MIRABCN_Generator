//
//  oscillatorBankGroup.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 13/01/2017.
//
//

#ifndef oscillatorBankGroup_h
#define oscillatorBankGroup_h

#include "oscillatorBank.h"

class oscillatorBankGroup{
public:
    oscillatorBankGroup(int oscillatorBankSize, int numOfOscillatorBanks, int id = 1, ofPoint pos = ofPoint(-1, -1));
    ~oscillatorBankGroup(){};
    
    
private:
    
    void parameterChanged(ofAbstractParameter &p);
    void oscillatorBankResult(int &oscBankInfo);
    void phasorInListener(float &phase);
    
    ofParameterGroup    *parameters;
    
    //INDEX
    ofParameter<float>  numWaves_Param; //Desphase Quantity
    ofParameter<float>   invert_Param;
    ofParameter<int>    symmetry_Param;
    ofParameter<float>  indexRand_Param;
    ofParameter<int>    indexOffset_Param;
    ofParameter<int>    indexQuant_Param;
    ofParameter<float>  combination_Param;
    ofParameter<int>    modulo_Param;
    //OSCILLATROS
    ofParameter<float>  phasorIn;
    ofParameter<int>    pow_Param; //Pow of the funcion, working on sin, cos....
    ofParameter<float>  pwm_Param;
    ofParameter<float>  holdTime_Param; //The duration of the hold in percentage (0.5) --> 50% of the cycle is the phase in initPhase
    ofParameter<float>  phaseOffset_Param;
    ofParameter<int>    quant_Param;
    ofParameter<float>  scale_Param;
    ofParameter<float>  offset_Param;
    ofParameter<float>  randomAdd_Param;
    ofParameter<int>    waveSelect_Param;
    ofParameter<float>  amplitude_Param;
    
    //Vectorized
    //INDEX
    ofParameter<vector<float>>  numWaves_vecParam;
    ofParameter<vector<float>>   invert_vecParam;
    ofParameter<vector<float>>    symmetry_vecParam;
    ofParameter<vector<float>>  indexRand_vecParam;
    ofParameter<vector<float>>    indexOffset_vecParam;
    ofParameter<vector<float>>    indexQuant_vecParam;
    ofParameter<vector<float>>  combination_vecParam;
    ofParameter<vector<float>>    modulo_vecParam;
    //OSCILLATROS
    ofParameter<vector<float>>    pow_vecParam;
    ofParameter<vector<float>>  pwm_vecParam;
    ofParameter<vector<float>>  holdTime_vecParam;
    ofParameter<vector<float>>  phaseOffset_vecParam;
    ofParameter<vector<float>>    quant_vecParam;
    ofParameter<vector<float>>  scale_vecParam;
    ofParameter<vector<float>>  offset_vecParam;
    ofParameter<vector<float>>  randomAdd_vecParam;
    ofParameter<vector<float>>    waveSelect_vecParam;
    ofParameter<vector<float>>  amplitude_vecParam;
    
    
    ofParameter<vector<vector<float>>>  bankGroupOut;
    ofParameter<vector<float>>          previewOut;
    
    vector<oscillatorBank*>  oscillatorBanks;
    
    vector<vector<float>> result;
    vector<int> resultFilledChecker;
};

#endif /* oscillatorBankGroup_h */
