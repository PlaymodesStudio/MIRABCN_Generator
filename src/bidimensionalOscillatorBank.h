//
//  bidimensionalOscillatorBank.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 15/11/2017.
//
//

#ifndef bidimensionalOscillatorBank_h
#define bidimensionalOscillatorBank_h

#include "baseIndexer.h"
#include "baseOscillator.h"

class bidimensionalOscillatorBank{
public:
    bidimensionalOscillatorBank(int bankId, int xSize, int ySize, ofPoint pos = ofPoint(-1, -1));
    ~bidimensionalOscillatorBank(){};
    
    vector<float>   computeBank(float phasor);
    void computeBidirectionalIndexs();
private:
    void parameterChanged(ofAbstractParameter &p);
    
    void newPhasorIn(float &f);
    void newPowParam(float &f);
    void newpulseWidthParam(float &f);
    void newHoldTimeParam(float &f);
    void newPhaseOffsetParam(float &f);
    void newQuantParam(int &i);
    void newScaleParam(float &f);
    void newOffsetParam(float &f);
    void newRandomAddParam(float &f);
    void newWaveSelectParam(int &i);
    void newAmplitudeParam(float &f);
    void newInvertParam(float &f);
    void newSkewParam(float &f);
    void newBiPowParam(float &f);
    
    vector<baseIndexer*> indexers;
    
    vector<baseOscillator*> oscillators;
    
    ofParameterGroup* parameters;

    
    ofParameter<float>    phasorIn;
    ofParameter<float>    pow_Param; //Pow of the funcion, working on sin, cos....
    ofParameter<float>    pulseWidth_Param;
    ofParameter<float>    holdTime_Param; //The duration of the hold in percentage (0.5) --> 50% of the cycle is the phase in initPhase
    ofParameter<float>    phaseOffset_Param;
    ofParameter<int>      quant_Param;
    ofParameter<float>    scale_Param;
    ofParameter<float>    offset_Param;
    ofParameter<float>    randomAdd_Param;
    ofParameter<float>    biPow_Param;
    ofParameter<int>      waveSelect_Param;
    ofParameter<float>    amplitude_Param;
    ofParameter<float>    invert_Param;
    ofParameter<float>    skew_Param;
    
    ofParameter<vector<float>>      oscillatorOut;
    
    vector<float> result;
    vector<int> resultFilledChecker;
};

#endif /* bidimensionalOscillatorBank_h */
