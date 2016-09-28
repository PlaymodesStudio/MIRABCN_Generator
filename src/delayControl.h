//
//  delayControl.h
//  DGTL_Generator
//
//  Created by iClon Media  on 02/08/16.
//
//

#ifndef delayControl_h
#define delayControl_h

#include "ofMain.h"

class delayControl{
public:
    delayControl(){};
    ~delayControl(){};
    
    void setup();
    int computeFunc(int index);
    void applyDelayToTexture(ofFbo &fbo, vector<float> infoVec, float bpm);
    
    void setIndexCount(int indexCount){indexCount_Param = indexCount;};
    
    
    ofParameterGroup getParameterGroup(){return parameters;};
    
private:
    
    //Parameters
    ofParameterGroup    parameters;
    ofParameter<int>    indexCount_Param; //The max number you will get from index
    ofParameter<bool>   invert_Param;
    ofParameter<int>    symmetry_Param;
    ofParameter<float>  comb_Param;
    ofParameter<int>    delay_frames;
    ofParameter<int>    delay_sixteenth;
    
    //Buffer of intoVec, we use deque to be able to create a circular buffer, erase the old values
    deque<vector<float>> infoVecBuffer;
};

#endif /* delayControl_h */
