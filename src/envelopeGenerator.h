//
//  envelopeGenerator.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 12/07/2017.
//
//

#ifndef envelopeGenerator_h
#define envelopeGenerator_h

#include "ofMain.h"
#include "ofxTweenzor.h"

class envelopeGenerator {
public:
    envelopeGenerator(int id, ofPoint pos = ofPoint(-1, -1));
    ~envelopeGenerator(){};
    
    
private:
    void update(ofEventArgs e);
    void gateInChanged(vector<float> &vf);
    void attackEnd(float *f);
    
    ofParameterGroup *parameters;
    ofParameter<float>  attack;
    ofParameter<float>  decay;
    ofParameter<float>  sustain;
    ofParameter<float>  release;
    
    ofParameter<int>  attackCurve;
    ofParameter<int>  decayCurve;
    ofParameter<int>  releaseCurve;
    
    ofParameter<vector<float>> gateIn;
    ofParameter<vector<float>> output;
    
    vector<float>   oldGateIn;
    vector<float>   outputComputeVec;
};

#endif /* envelopeGenerator_h */
