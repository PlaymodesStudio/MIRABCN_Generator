//
//  oscInput.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 06/09/2017.
//
//

#ifndef oscInput_h
#define oscInput_h

#include "ofMain.h"
#include "ofxOsc.h"

class oscInput{
public:
    oscInput();
    ~oscInput(){};
    
private:
    void update(ofEventArgs &args);
    
    ofParameterGroup *parameters;
    ofParameter<string> oscPort;
    vector<ofParameter<vector<float>>> individualOutputs;
    ofParameter<vector<float>>  joinedOutput;
    
    ofxOscReceiver oscReceiver;
};

#endif /* oscInput_h */
