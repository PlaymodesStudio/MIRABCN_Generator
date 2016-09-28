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

class bpmControl: public ofBaseSoundInput{
public:
    bpmControl(){};
    ~bpmControl(){};
    void setup();
    
    float getBpm(){return beat.bpm;};
    void audioIn(float * input, int bufferSize, int nChannels);
    
private:
    ofxAubioBeat beat;
        
    ofSoundStream soundStream;
    
};

#endif /* bpmControl_hpp */
