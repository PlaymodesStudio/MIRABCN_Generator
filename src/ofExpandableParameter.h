//
//  ofExpandableParameter.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 10/01/2017.
//
//

#ifndef ofExpandableParameter_h
#define ofExpandableParameter_h

#include "ofParameter.h"

template<typename ParameterType>
class ofExpandableParameter : public ofParameter<ParameterType>{
public:
    vector<ParameterType> getExpandedValue(){return expandedValue;};
private:
    vector<ParameterType> expandedValue;
};

#endif /* ofExpandableParameter_h */
