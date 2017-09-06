//
//  vectorGetter.hpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 06/09/2017.
//
//

#ifndef vectorGetter_h
#define vectorGetter_h

#include "ofMain.h"

class vectorGetter{
public:
    vectorGetter(int id, ofPoint pos = ofPoint(-1, -1));
    ~vectorGetter(){}
    
private:
    void inputListener(vector<float> &v);
    
    ofParameterGroup *parameters;
    ofParameter<vector<float>>  input;
    ofParameter<string> index;
    ofParameter<float>  output;
};

#endif /* vectorGetter_h */
