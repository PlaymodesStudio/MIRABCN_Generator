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
    void computeOutput(ofTexture* &in);
    
    ofParameterGroup *parameters;
    ofParameter<int>    triggerTextureIndex;
    vector<ofParameter<ofTexture*>>    inputs;
    ofParameter<ofTexture*>    output;
    
    ofFbo outputFbo;
};

#endif /* textureUnifier_h */
