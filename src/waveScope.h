//
//  waveScope.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 10/01/2017.
//
//

#ifndef waveScope_h
#define waveScope_h

#include "ofMain.h"
#include "parametersControl.h"

class waveScope{
public:
    waveScope();
    ~waveScope(){};
    
    void draw();
private:
    ofFbo computeLinWaveTex(vector<float> values);
    
    ofParameterGroup* parameters;
    vector<ofParameter<vector<float>>> oscillatorBankIns;
    ofParameter<vector<vector<float>>> mainOutIn;
};

#endif /* waveScope_h */
