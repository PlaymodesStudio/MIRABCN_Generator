//
//  chartresTextureUnifier.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 29/08/2017.
//
//

#ifndef chartresTextureUnifier_h
#define chartresTextureUnifier_h

#include "ofMain.h"

class chartresTextureUnifier{
public:
    chartresTextureUnifier();
    ~chartresTextureUnifier(){};
    
    
private:
    void computeOutput(vector<vector<ofColor>> &in);
    
    ofParameterGroup *parameters;
    ofParameter<vector<vector<ofColor>>>    in288_8;
    ofParameter<vector<vector<ofColor>>>    in1008_6;
    ofParameter<vector<vector<ofColor>>>    in8_2;
    ofParameter<vector<vector<ofColor>>>    out;
};

#endif /* chartresTextureUnifier_h */
