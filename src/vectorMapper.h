//
//  vectorMapper.h
//  DGTL_Generator
//
//  Created by Eduard Frigola on 28/07/16.
//
//

#ifndef vectorMapper_h
#define vectorMapper_h

#pragma once

#include "ofMain.h"


class vectorMapper{
public:
    vectorMapper(int index = 0, ofPoint pos = ofPoint(-1, -1));
    ~vectorMapper();
    void setup(int index = 0);
    
    void recalculate(vector<float>& vf);

private:
    
    ofParameterGroup*    parameters;
    ofParameter<vector<float>>  Input;
    ofParameter<float>  MinInput;
    ofParameter<float>  MaxInput;
    ofParameter<float>  MinOutput;
    ofParameter<float>  MaxOutput;
    ofParameter<vector<float>>  Output;

};


#endif /* mapper_h */
