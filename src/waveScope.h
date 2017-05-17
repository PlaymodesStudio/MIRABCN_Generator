//
//  waveScope.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 10/01/2017.
//
//

#ifndef waveScope_h
#define waveScope_h

#include "ofMain.h"
#include "parametersControl.h"
#include "bufferLoggerChannel.h"

class waveScope{
public:
    waveScope(shared_ptr<bufferLoggerChannel> logBuffer_ = nullptr,  int numBankScopes = 2, ofPoint pos = ofPoint(-1, -1));
    ~waveScope(){delete parameters;};
    
    void draw();
    
    void mouseMoved(ofMouseEventArgs &a);
    void mousePressed(ofMouseEventArgs &a);
    void mouseReleased(ofMouseEventArgs &a);
    void mouseDragged(ofMouseEventArgs &a);
private:
    
    ofParameterGroup* parameters;
    vector<ofParameter<vector<float>>> oscillatorBankIns;
    ofParameter<vector<vector<float>>> mainOutIn;
    
    shared_ptr<bufferLoggerChannel> logBuffer;
    
    int contentWidthOffset;
    int mousePressInititalX;
    bool isInMovableRegion;
};

#endif /* waveScope_h */
