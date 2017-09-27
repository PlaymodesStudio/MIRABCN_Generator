//
//  textureUnifier.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 22/09/2017.
//
//

#ifndef textureUnifier_h
#define textureUnifier_h

#include "ofMain.h"

class textureUnifier{
public:
    textureUnifier(int numInputs);
    ~textureUnifier(){};
    
    
private:
    void computeOutput(vector<vector<ofColor>> &in);
    
    ofParameterGroup *parameters;
    ofParameter<int>    triggerTextureIndex;
    vector<ofParameter<vector<vector<ofColor>>>>    inputs;
    ofParameter<vector<vector<ofColor>>>    output;
};

#endif /* textureUnifier_h */
