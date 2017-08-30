//
//  manualOscillatorBank.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 30/08/2017.
//
//

#ifndef manualOscillatorBank_h
#define manualOscillatorBank_h

#include "baseIndexer.h"

class manualOscillatorBank: public baseIndexer{
public:
    manualOscillatorBank(int nOscillators, int _bankId = 0, ofPoint pos = ofPoint(-1, -1));
    ~manualOscillatorBank(){};
    
    
private:
    void manualInputChanged(float &f);
    void computeValues(float &f);
    
    
    ofParameter<float>  manualInput;
    deque<float>       buffer;
    vector<vector<float>>   auxiliaryBuffers;
    int                 bufferIndex;
    ofParameter<float>  phasorIn;
    float               oldPhasor;
    ofParameter<int>    delay;
    ofParameter<vector<float>>  output;
};

#endif /* manualOscillatorBank_h */
