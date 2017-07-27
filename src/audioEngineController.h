//
//  audioEngineController.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 26/07/2017.
//
//

#ifndef audioEngineController_h
#define audioEngineController_h

#include "ofMain.h"
#include "ofxOsc.h"

class audioEngineController {
public:
    audioEngineController();
    
private:
    void oscHostAndPortListener(string &s);
    
    void presetNumSender(int &i);
    void param_aSender(float &f);
    void param_bSender(float &f);
    void param_cSender(float &f);
    void param_vaSender(vector<float> &vf);
    void param_vbSender(vector<float> &vf);
    void param_vcSender(vector<float> &vf);
    
    ofParameterGroup* parameters;
    
    ofParameter<string> oscHost;
    ofParameter<string> oscPort;
    ofParameter<int>    presetNum;
    ofParameter<float>  param_a;
    ofParameter<float>  param_b;
    ofParameter<float>  param_c;
    ofParameter<vector<float>>  param_va;
    ofParameter<vector<float>>  param_vb;
    ofParameter<vector<float>>  param_vc;
    
    ofxOscSender oscSender;
};

#endif /* audioEngineController_h */
