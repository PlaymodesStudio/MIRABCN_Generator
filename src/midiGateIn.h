//
//  midiGateIn.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 14/07/2017.
//
//

#ifndef midiGateIn_h
#define midiGateIn_h

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxOsc.h"

class midiGateIn : ofxMidiListener{
public:
    midiGateIn(int _id, ofPoint pos = ofPoint(-1, -1));
    ~midiGateIn(){};
    
private:
    void newMidiMessage(ofxMidiMessage& eventArgs);
    void midiDeviceListener(int &device);
    void noteRangeChanged(int &note);
    
    ofParameterGroup*   parameters;
    ofParameter<int>    midiDevice;
    ofParameter<int>    midiChannel;
    ofParameter<int>    noteOnStart;
    ofParameter<int>    noteOnEnd;
    
    ofParameter<vector<float>> output;
    
    ofxMidiIn   midiIn;
    vector<float>   outputStore;
};

#endif /* midiGateIn_h */
