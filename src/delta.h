//
//  delta.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 14/07/2017.
//
//

#ifndef delta_h
#define delta_h

#include "ofMain.h"

class delta{
public:
    delta(int id_, ofPoint pos = ofPoint(-1, -1));
    ~delta(){};
    
private:
    void computeOutput(vector<float> &in);
    
    ofParameterGroup*   parameters;
    ofParameter<float>  gain;
    ofParameter<bool>   invert;
    ofParameter<float>  smooth;
    ofParameter<vector<float>>   input;
    ofParameter<vector<float>>   output;
    
    vector<float>   inputStore;
    vector<float>  outputStore;
};

#endif /* delta_h */
