//
//  dataRecorder.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 31/08/2017.
//
//

#ifndef dataRecorder_h
#define dataRecorder_h

#include "ofMain.h"

class dataRecorder{
public:
    dataRecorder();
    ~dataRecorder(){};
    
private:
    void phasorInListener(float &f);
    void inputListener(vector<vector<ofColor>> &info);
    void recordListener(bool &b);
    
    ofParameterGroup*   parameters;
    ofParameter<float>  phasorIn;
    ofParameter<bool>   record;
    ofParameter<bool>   autoRecLoop;
    ofParameter<bool>   invert;
    ofParameter<string> filename;
    ofParameter<vector<vector<ofColor>>>    input;
    
    float oldPhasor;
    int frameCounter;
};

#endif /* dataRecorder_h */
