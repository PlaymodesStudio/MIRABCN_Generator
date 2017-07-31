//
//  phasorClass.h
//  DGTL_Generator
//
//  Created by Eduard Frigola on 28/07/16.
//
//

#ifndef mapper_h
#define mapper_h

#pragma once

#include "ofMain.h"


class mapper{
public:
    mapper(int index = 0, ofPoint pos = ofPoint(-1, -1));
    ~mapper();
    void setup(int index = 0);
    float getRange();    
    void resetRange();
    
    ofParameterGroup* getParameterGroup(){return parameters;};
    void recalculate(float& f);

private:
    
    ofParameterGroup*    parameters;
    ofParameter<float>  Input;
    ofParameter<float>  MinInput;
    ofParameter<float>  MaxInput;
    ofParameter<float>  MinOutput;
    ofParameter<float>  MaxOutput;
    ofParameter<float>  Output;

};


#endif /* mapper_h */
