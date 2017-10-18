//
//  bankDimensionCombinator.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 18/10/2017.
//
//

#ifndef bankDimensionCombinator_h
#define bankDimensionCombinator_h

#include "ofMain.h"

class bankDimensionCombinator{
public:
    bankDimensionCombinator();
    ~bankDimensionCombinator();
    
private:
    void paramListener(vector<float> &vf);
    
    ofParameterGroup    *parameters;
    ofParameter<vector<float>>  x_input;
    ofParameter<vector<float>>  y_input;
    ofParameter<int>    operationSelector;
    
    ofParameter<vector<float>>  output;
};

#endif /* bankDimensionCombinator_h */
