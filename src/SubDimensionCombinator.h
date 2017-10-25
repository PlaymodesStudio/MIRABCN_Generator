//
//  SubDimensionCombinator.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 25/10/2017.
//
//

#ifndef SubDimensionCombinator_h
#define SubDimensionCombinator_h

#include "ofMain.h"

class subDimensionCombinator{
public:
    subDimensionCombinator();
    ~subDimensionCombinator();
    
private:
    void paramListener(vector<float> &vf);
    
    ofParameterGroup    *parameters;
    ofParameter<vector<float>>  original;
    ofParameter<vector<float>>  subDimension;
    
    ofParameter<vector<float>>  output;
};


#endif /* SubDimensionCombinator_h */
