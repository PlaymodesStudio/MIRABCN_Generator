//
//  bpmControl.h
//  DGTL_Generator
//
//  Created by Eduard Frigola on 12/08/16.
//
//

#ifndef bpmControl_h
#define bpmControl_h

#include "ofMain.h"
#include "ofxAubio.h"

class bpmControl{
public:
    bpmControl(){};
    ~bpmControl(){};
    
    static bpmControl &getInstance()
    {
        static bpmControl instance;
        return instance;
    }
    
    void setup();
    
    float getBpm(){return beat.bpm;};
    void audioIn(float * input, int bufferSize, int nChannels);
    
    ofEvent<float> bpmChanged;
    
private:
    ofxAubioBeat beat;
    float oldBpm;
    
};

#endif /* bpmControl_hpp */
