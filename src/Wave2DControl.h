//
//  Wave2DControl.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 28/09/16.
//
//

#ifndef Wave2DControl_h
#define Wave2DControl_h

#include "ofMain.h"

class Wave2DControl{
public:
    Wave2DControl(){};
    ~Wave2DControl(){};
    
    void setup(int width, int height);
    int computeFunc(int index);
    void applyDelayToTexture(ofFbo &fbo, vector<float> infoVec, float bpm, float phasor);
    
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
    
    int width;
    int height;
    
    vector<vector<float>> grid;
    vector<pair<ofVec2f, float>> barInfo_Pos;
    
    //Buffer of intoVec, we use deque to be able to create a circular buffer, erase the old values
    deque<vector<float>> infoVecBuffer;
};


#endif /* Wave2DControl_h */
