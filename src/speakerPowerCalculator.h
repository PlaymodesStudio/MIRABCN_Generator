//
//  speakerPowerCalculator.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 27/09/2017.
//
//

#ifndef speakerPowerCalculator_h
#define speakerPowerCalculator_h

#include "ofMain.h"

class speakerPowerCalculator{
public:
    speakerPowerCalculator(int lenght);
    ~speakerPowerCalculator();
    
private:
    void inputListener(vector<vector<ofColor>> &in);
    
    
    vector<vector<float>>   widthManualWindowing;
    vector<float>   heightManualWindowing;
    
    int width, height;
    ofParameterGroup*   parameters;
    ofParameter<int>    computeFunc;
    ofParameter<vector<vector<ofColor>>>    input;
    ofParameter<vector<vector<float>>> output;
};

#endif /* speakerPowerCalculator_h */
