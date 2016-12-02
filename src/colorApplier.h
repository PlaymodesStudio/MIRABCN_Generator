//
//  colorApplier.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 01/12/2016.
//
//

#ifndef colorApplier_h
#define colorApplier_h

#include "ofMain.h"
#include "parametersControl.h"

class colorApplier{
    
public:
    
    colorApplier();
    ~colorApplier(){};
    
    
private:
    ofParameterGroup        parameters;
    ofParameter<void>       cableConnected;
    ofParameter<ofColor>    colorPickerParam[2];
    ofParameter<int>        colorRParam[2];
    ofParameter<int>        colorGParam[2];
    ofParameter<int>        colorBParam[2];
    
    ofParameter<int>        randomColorStepsParam;
    ofParameter<int>        randomizeTypeColorParam; //Select the rand way: in change preset, in phasor triggered...
    
    ofParameter<vector<float>>  indexs;
};

#endif /* colorApplier_h */
