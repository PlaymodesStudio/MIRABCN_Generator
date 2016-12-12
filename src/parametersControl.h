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
#include "bpmControl.h"

static const int NUM_PRESETS = 40;

class nodeConnection{
public:
    nodeConnection(){};
    nodeConnection(ofxDatGuiComponent* c){
        points.resize(2);
        points[0].x = c->getX() + c->getWidth();
        points[0].y = c->getY() + c->getHeight()/2;
        bindedComponents[0] = c;
        polyline.addVertex(points[0]);
        polyline.addVertex(points[0]);
    }
    
    void moveLine(ofPoint p){
        points[1] = p;
        polyline.getVertices()[1] = p;
    }
    
    void connectTo(ofxDatGuiComponent* c){
        points[1].x = c->getX();
        points[1].y = c->getY() + c->getHeight()/2;
        bindedComponents[1] = c;
        polyline.getVertices()[1] = points[1];
        closedLine = true;
    }
    
    ofPolyline getPolyline(){
        if(closedLine){
            ofPoint p1;
            p1.x = bindedComponents[0]->getX() + bindedComponents[0]->getWidth();
            p1.y = bindedComponents[0]->getY() + bindedComponents[0]->getHeight()/2;
            ofPoint p2;
            p2.x = bindedComponents[1]->getX();
            p2.y = bindedComponents[1]->getY() + bindedComponents[1]->getHeight()/2;
            if(p1 != points[0] || p2 != points[1]){
                polyline.getVertices()[0] = p1;
                polyline.getVertices()[1] = p2;
            }
        }
        return polyline;
    }
    
    
    bool closedLine = false;
    
private:
    vector<ofPoint> points;
    ofPolyline polyline;
    ofxDatGuiComponent* bindedComponents[2];
};


class parametersControl: public ofxMidiListener{
public:
    
    parametersControl(){};
    ~parametersControl(){};
    
    static parametersControl &getInstance()
    {
        static parametersControl instance;
        return instance;
    }
    
    void createGuiFromParams(ofParameterGroup paramGroup, ofColor guiColor = ofColor(ofRandom(255), ofRandom(255), ofRandom(255)));
    
    void setup();
    void update();
    void draw();
    
    int getGuiWidth(){return datGui->getWidth();};
    
    void onGuiButtonEvent(ofxDatGuiButtonEvent e);
    void onGuiToggleEvent(ofxDatGuiToggleEvent e);
    void onGuiDropdownEvent(ofxDatGuiDropdownEvent e);
    void onGuiMatrixEvent(ofxDatGuiMatrixEvent e);
    void onGuiSliderEvent(ofxDatGuiSliderEvent e);
    void onGuiTextInputEvent(ofxDatGuiTextInputEvent e);
    void onGuiColorPickerEvent(ofxDatGuiColorPickerEvent e);
    void onGuiRightClickEvent(ofxDatGuiRightClickEvent e);
    
    void mouseDragged(ofMouseEventArgs &e);
    void mouseReleased(ofMouseEventArgs &e);
    
    void bpmChangedListener(float &bpm);
    
    void listenerFunction(ofAbstractParameter& e);
    
    void newMidiMessage(ofxMidiMessage& eventArgs);
    
    void savePreset(int presetNum, string bank);
    void loadPreset(int presetNum, string bank);
    void loadPresetWithFade(int presetNum, string bank);
    
    void saveGuiArrangement();
    void loadGuiArrangement();
    
    bool loadPresetsSequence();
    
    void loadPresetWhenFadeOutCompletes(float *arg);
    
    void setWindows(shared_ptr<ofAppBaseWindow> guiWindow, shared_ptr<ofAppBaseWindow> prevWindow){this->guiWindow = guiWindow; this->prevWindow = prevWindow;};
    
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
    
    shared_ptr<ofAppBaseWindow> guiWindow;
    shared_ptr<ofAppBaseWindow> prevWindow;
    
    
    //BPM Detect
    bpmControl *beatTracker;
    float       newBpm;
    
    
    //node
    vector<nodeConnection>  connections;
};


#endif /* parametersControl_h */
