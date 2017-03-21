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

enum moduleType{
    phasor_module = 1,
    oscillator_module = 2,
    oscillatorBank_module = 3,
    oscillatorBankGroup_module = 4
};

static const int NUM_PRESETS = 40;

class nodeConnection{
public:
    nodeConnection(){};
    nodeConnection(ofxDatGuiComponent* c, ofAbstractParameter* p){
        points.resize(2);
        points[0].x = c->getX() + c->getWidth();
        points[0].y = c->getY() + c->getHeight()/2;
        bindedComponents[0] = c;
        bindedParameters[0] = p;
        path.setFilled(false);
        path.setStrokeColor(ofColor::white);
        path.setStrokeWidth(1);
        path.setCurveResolution(50);
        path.moveTo(points[0]);
    }
    
    void moveLine(ofPoint p){
        points[1] = p;
        float distance = abs(points[0].x - points[1].x);
        path.clear();
        path.moveTo(points[0]);
        path.bezierTo(points[0]+ofPoint(distance/3,0), points[1]-ofPoint(distance/3,0), points[1]);
    }
    
    void connectTo(ofxDatGuiComponent* c, ofAbstractParameter* p){
        points[1].x = c->getX();
        points[1].y = c->getY() + c->getHeight()/2;
        bindedComponents[1] = c;
        bindedParameters[1] = p;
        float distance = abs(points[0].x - points[1].x);
        path.clear();
        path.moveTo(points[0]);
        path.bezierTo(points[0]+ofPoint(distance/3,0), points[1]-ofPoint(distance/3,0), points[1]);
        closedLine = true;
    }
    
    void disconnect(){
        closedLine = false;
        bindedParameters[1] = nullptr;
        bindedComponents[1] = nullptr;
    }
    
    ofPath getPolyline(){
        if(closedLine){
            ofPoint p1;
            p1.x = bindedComponents[0]->getX() + bindedComponents[0]->getWidth();
            p1.y = bindedComponents[0]->getY() + bindedComponents[0]->getHeight()/2;
            ofPoint p2;
            p2.x = bindedComponents[1]->getX();
            p2.y = bindedComponents[1]->getY() + bindedComponents[1]->getHeight()/2;
            if(p1 != points[0] || p2 != points[1]){
                points = {p1, p2};
                float distance = abs(points[0].x - points[1].x);
                path.clear();
                path.moveTo(points[0]);
                path.bezierTo(points[0]+ofPoint(distance/3,0), points[1]-ofPoint(distance/3,0), points[1]);
            }
        }
        
        return path;
    }
    
    
    
    ofAbstractParameter* getSourceParameter(){return bindedParameters[0];};
    ofAbstractParameter* getSinkParameter(){return bindedParameters[1];};
    
    bool closedLine = false;
    
private:
    vector<ofPoint> points;
    ofPath path;
    ofxDatGuiComponent* bindedComponents[2];
    ofAbstractParameter* bindedParameters[2];
    ofColor color = ofColor::white;
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
    
    void createGuiFromParams(ofParameterGroup *paramGroup, ofColor guiColor = ofColor(ofRandom(255), ofRandom(255), ofRandom(255)), ofPoint pos = ofPoint(-1, -1));
    
    void setup();
    void update(ofEventArgs &args);
    void draw(ofEventArgs &args);
    
    int getGuiWidth(){return datGui->getWidth();};
    
    void onGuiButtonEvent(ofxDatGuiButtonEvent e);
    void onGuiToggleEvent(ofxDatGuiToggleEvent e);
    void onGuiDropdownEvent(ofxDatGuiDropdownEvent e);
    void onGuiMatrixEvent(ofxDatGuiMatrixEvent e);
    void onGuiSliderEvent(ofxDatGuiSliderEvent e);
    void onGuiTextInputEvent(ofxDatGuiTextInputEvent e);
    void onGuiColorPickerEvent(ofxDatGuiColorPickerEvent e);
    void onGuiRightClickEvent(ofxDatGuiRightClickEvent e);
    
    void newModuleListener(ofxDatGuiDropdownEvent e);
    
    void keyPressed(ofKeyEventArgs &e);
    void keyReleased(ofKeyEventArgs &e);
    void mouseMoved(ofMouseEventArgs &e);
    void mouseDragged(ofMouseEventArgs &e);
    void mousePressed(ofMouseEventArgs &e);
    void mouseReleased(ofMouseEventArgs &e);
    void mouseScrolled(ofMouseEventArgs &e);
    void mouseEntered(ofMouseEventArgs &e);
    void mouseExited(ofMouseEventArgs &e);
    
    
    
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
    
    ofEvent<pair<moduleType, ofPoint>> createNewModule;
    
private:
    
    void setFromNormalizedValue(ofAbstractParameter* p, float v);
    void setFromSameTypeValue(ofAbstractParameter* source, ofAbstractParameter* sink);
    
    ofxDatGui *datGui;
    ofxDatGuiMatrix* presetMatrix;
    ofxDatGuiDropdown* bankSelect;
    
    vector<ofxDatGui*> datGuis;
    vector<ofParameterGroup*> parameterGroups;
    
    ofxDatGui *popUpMenu;
    
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
    vector<shared_ptr<nodeConnection>>  connections;
    
    bool commandPressed = false;
    
    ofMatrix4x4 transformMatrix;
    ofPoint dragCanvasInitialPoint;
};


#endif /* parametersControl_h */
