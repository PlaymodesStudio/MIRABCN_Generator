//
//  parametersControl.h
//  DGTL_Generator
//
//  Created by Eduard Frigola on 03/08/16.
//
//

#ifndef parametersControl_h
#define parametersControl_h

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxDatGui.h"
#include "ofxMidi.h"

static const int NUM_PRESETS = 40;


class parametersControl: public ofxMidiListener{
public:
    
    parametersControl(){};
    ~parametersControl(){};
    
    void bindPhasorParams(ofParameterGroup paramGroup){phasorParams = paramGroup;};
    void bindOscilatorParams(ofParameterGroup paramGroup){oscilatorParams = paramGroup;};
    void bindDelayParams(ofParameterGroup paramGroup){delayParams = paramGroup;};
    
    void setup();
    void update();
    
    int getGuiWidth(){return datGui->getWidth();};
    
    void onGuiButtonEvent(ofxDatGuiButtonEvent e);
    void onGuiToggleEvent(ofxDatGuiToggleEvent e);
    void onGuiDropdownEvent(ofxDatGuiDropdownEvent e);
    void onGuiMatrixEvent(ofxDatGuiMatrixEvent e);
    void onGuiSliderEvent(ofxDatGuiSliderEvent e);
    
    void listenerFunction(ofAbstractParameter& e);
    
    void newMidiMessage(ofxMidiMessage& eventArgs);
    
    void savePreset(int presetNum);
    void loadPreset(int presetNum);
    
private:
    
    ofParameterGroup phasorParams;
    ofParameterGroup oscilatorParams;
    ofParameterGroup delayParams;
    
    ofxDatGui *datGui;
    ofxDatGuiMatrix* presetMatrix;
    
    ofParameter<bool> autoPreset;
    ofParameter<float> presetChangeBeatsPeriod;
    
    ofxOscReceiver oscReceiver;
    
    ofXml xml;
    
    ofxMidiOut midiOut;
    ofxMidiIn   midiIn;
    
    deque<ofxMidiMessage> midiMessages;
    
    float   presetChangedTimeStamp;
    float   periodTime;
    
    vector<int> randomPresetsArrange;
    int presetChangeCounter;
};


#endif /* parametersControl_h */
