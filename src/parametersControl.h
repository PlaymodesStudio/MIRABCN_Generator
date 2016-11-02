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
#include "ofxTweenzor.h"

static const int NUM_PRESETS = 40;


class parametersControl: public ofxMidiListener{
public:
    
    parametersControl(){};
    ~parametersControl(){};
    
    void createGuiFromParams(ofParameterGroup paramGroup);
    
    void setup();
    void update();
    
    int getGuiWidth(){return datGui->getWidth();};
    
    void onGuiButtonEvent(ofxDatGuiButtonEvent e);
    void onGuiToggleEvent(ofxDatGuiToggleEvent e);
    void onGuiDropdownEvent(ofxDatGuiDropdownEvent e);
    void onGuiMatrixEvent(ofxDatGuiMatrixEvent e);
    void onGuiSliderEvent(ofxDatGuiSliderEvent e);
    void onGuiTextInputEvent(ofxDatGuiTextInputEvent e);
    
    void listenerFunction(ofAbstractParameter& e);
    
    void newMidiMessage(ofxMidiMessage& eventArgs);
    
    void savePreset(int presetNum, string bank);
    void loadPreset(int presetNum, string bank);
    void loadPresetWithFade(int presetNum, string bank);
    
    void loadPresetWhenFadeOutCompletes(float *arg);
    
private:
    
    ofxDatGui *datGui;
    ofxDatGuiMatrix* presetMatrix;
    ofxDatGuiDropdown* bankSelect;
    
    vector<ofxDatGui*> datGuis;
    vector<ofParameterGroup> parameterGroups;
    
    ofParameter<bool> autoPreset;
    ofParameter<float> presetChangeBeatsPeriod;
    ofParameter<float>  fadeTime;
    
    ofxOscReceiver oscReceiver;
    ofxOscSender oscSender;
    
    ofXml xml;
    
    ofxMidiOut midiOut;
    ofxMidiIn   midiIn;
    
    deque<ofxMidiMessage> midiMessages;
    
    float   presetChangedTimeStamp;
    float   periodTime;
    
    vector<int> randomPresetsArrange;
    vector<pair<int, string>> presetNumbersAndBanks;
    vector<int> presetsTime;
    int presetChangeCounter;
    
    int presetToLoad;
    string bankToLoad;
    
    bool isFading;
};


#endif /* parametersControl_h */
