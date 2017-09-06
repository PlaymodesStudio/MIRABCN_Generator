//
//  vectorChain.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 06/09/2017.
//
//

#ifndef vectorChain_h
#define vectorChain_h

#include "ofMain.h"

class vectorChain{
public:
    vectorChain(int nInputs, int id, ofPoint pos = ofPoint(-1, -1));
    ~vectorChain(){};
    
private:
    void inputListener(vector<float> &v);
    
    ofParameterGroup *parameters;
    vector<ofParameter<vector<float>>>  inputs;
    ofParameter<float>  offset;
    ofParameter<vector<float>>  output;
};

#endif /* vectorChain_h */
