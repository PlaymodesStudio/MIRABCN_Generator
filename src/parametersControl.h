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
    colorApplier_module = 4,
    waveScope_module = 5
};

static const int NUM_PRESETS = 40;

class nodeConnection{
public:
    nodeConnection(){};
    nodeConnection(ofxDatGuiComponent* c, shared_ptr<ofxDatGui> g, ofAbstractParameter* p){
        min.set("Min", 0, 0, 1);
        max.set("Max", 1, 0, 1);
        points.resize(2);
        points[0].x = c->getX() + c->getWidth();
        points[0].y = c->getY() + c->getHeight()/2;
        bindedComponents[0] = c;
        bindedComponentsParent[0] = g;
        bindedParameters[0] = p;
        path.setFilled(false);
        path.setStrokeColor(ofColor::white);
        path.setStrokeWidth(1);
        path.setCurveResolution(50);
        path.moveTo(points[0]);
    }
    
    ~nodeConnection(){
        if(closedLine)
            disconnect();
    }
    
    void moveLine(ofPoint p){
        points[1] = p;
        float distance = abs(points[0].x - points[1].x);
        float x =(points[0].y - points[1].y);
        float controlPointShift = 20*x/(pow(x,2)+0.1)+1;
        path.clear();
        path.moveTo(points[0]);
        path.bezierTo(points[0]+ofPoint(distance/3,0), points[1]-ofPoint(distance/3,0), points[1]);
//        path.bezierTo(points[0]+ofPoint(distance/3,controlPointShift*10), points[1]-ofPoint(distance/3,controlPointShift*10), points[1]);
    }
    
    void connectTo(ofxDatGuiComponent* c, shared_ptr<ofxDatGui> g, ofAbstractParameter* p){
        gui = new ofxDatGui();
        gui->setVisible(false);
        gui->addLabel(bindedParameters[0]->getName() + " ==> " + p->getName());
        gui->addSlider(min);
        gui->addSlider(max);
        points[1].x = c->getX();
        points[1].y = c->getY() + c->getHeight()/2;
        bindedComponents[1] = c;
        bindedComponentsParent[1] = g;
        bindedParameters[1] = p;
        float distance = abs(points[0].x - points[1].x);
        float x =(points[0].y - points[1].y);
        float controlPointShift = 0;//20*x/(pow(x,2)+0.1)+2;
        path.clear();
        path.moveTo(points[0]);
        path.bezierTo(points[0]+ofPoint(distance/3,controlPointShift*10), points[1]-ofPoint(distance/3,controlPointShift*10), points[1]);

        closedLine = true;
    }
    
    void disconnect(){
        ofNotifyEvent(destroyEvent, *bindedParameters[1], this);
        closedLine = false;
        bindedParameters[1] = nullptr;
        bindedComponents[1] = nullptr;
        toggleGui(false);
        delete gui;
        gui = nullptr;
    }
    
    ofPath getPath(){
        if(closedLine){
            ofPoint p1;
            if(bindedComponentsParent[0]->getExpanded()){
                p1.x = bindedComponents[0]->getX() + bindedComponents[0]->getWidth();
                p1.y = bindedComponents[0]->getY() + bindedComponents[0]->getHeight()/2;
            }else{
                p1.x = bindedComponentsParent[0]->getHeader()->getX() + bindedComponentsParent[0]->getHeader()->getWidth();
                p1.y = bindedComponentsParent[0]->getHeader()->getY() + bindedComponentsParent[0]->getHeader()->getHeight()/2;
            }
            ofPoint p2;
            if(bindedComponentsParent[1]->getExpanded()){
                p2.x = bindedComponents[1]->getX();
                p2.y = bindedComponents[1]->getY() + bindedComponents[1]->getHeight()/2;
            }else{
                p2.x = bindedComponentsParent[1]->getHeader()->getX();
                p2.y = bindedComponentsParent[1]->getHeader()->getY() + bindedComponentsParent[1]->getHeader()->getHeight()/2;
            }
            
            if(p1 != points[0] || p2 != points[1]){
                if(gui->getVisible() == true) toggleGui(false);
                points = {p1, p2};
                float distance = abs(points[0].x - points[1].x);
                float x =(points[0].y - points[1].y);
                float controlPointShift = 0;//20*x/(pow(x,2)+0.5)-2;
                path.clear();
                path.moveTo(points[0]);
                path.bezierTo(points[0]+ofPoint(distance/3,controlPointShift*10), points[1]-ofPoint(distance/3,controlPointShift*10), points[1]);
            }
        }
        
        return path;
    }
    
    bool hitTest(ofPoint p){
        int margin = 10;
        bool c = false;
        if(closedLine){
            for (auto polyline : path.getOutline()){
                for(int i = -margin; i < margin; i++){
                    for ( int j = -margin; j < margin; j++){
                        if(polyline.inside(p+ofPoint(i, j)))
                            c = true;
                    }
                }
            }
        }
        return c;
    }
    
    void toggleGui(bool visible, ofPoint pos = ofPoint(-1, -1)){
        if(gui != nullptr){
            if(visible){
                path.setStrokeWidth(3);
                path.setStrokeColor(ofColor::lightGray);
                gui->setVisible(true);
                if(pos != ofPoint(-1, -1));
                   gui->setPosition(pos.x, pos.y);
            }else{
                path.setStrokeWidth(1);
                path.setStrokeColor(ofColor::white);
                gui->setVisible(false);
            }
        }
    }
    
    
    ofAbstractParameter* getSourceParameter(){return bindedParameters[0];};
    ofAbstractParameter* getSinkParameter(){return bindedParameters[1];};
    
    float getMin(){return min;};
    float getMax(){return max;};
    
    void setMin(float _min){min = _min;};
    void setMax(float _max){max = _max;};
    
    shared_ptr<ofxDatGui> getParentGuis(int i){return bindedComponentsParent[i];};
    
    bool closedLine = false;
    
    ofEvent<ofAbstractParameter> destroyEvent;
    
private:
    ofxDatGui*  gui;
    vector<ofPoint> points;
    ofPath path;
    ofxDatGuiComponent* bindedComponents[2];
    shared_ptr<ofxDatGui>          bindedComponentsParent[2];
    ofAbstractParameter* bindedParameters[2];
    ofColor color = ofColor::white;
    ofParameter<float> min = 0;
    ofParameter<float> max = 1;
};


class parametersControl: public ofxMidiListener{
public:
    
    parametersControl(){};
    ~parametersControl();
    
    static parametersControl &getInstance()
    {
        static parametersControl instance;
        return instance;
    }
    
    void createGuiFromParams(ofParameterGroup *paramGroup, ofColor guiColor = ofColor(ofRandom(255), ofRandom(255), ofRandom(255)), ofPoint pos = ofPoint(-1, -1));
    
    void setup();
    void update(ofEventArgs &args);
    void draw(ofEventArgs &args);
    
    void onGuiButtonEvent(ofxDatGuiButtonEvent e);
    void onGuiToggleEvent(ofxDatGuiToggleEvent e);
    void onGuiDropdownEvent(ofxDatGuiDropdownEvent e);
    void onGuiMatrixEvent(ofxDatGuiMatrixEvent e);
    void onGuiSliderEvent(ofxDatGuiSliderEvent e);
    void onGuiTextInputEvent(ofxDatGuiTextInputEvent e);
    void onGuiColorPickerEvent(ofxDatGuiColorPickerEvent e);
    void onGuiRightClickEvent(ofxDatGuiRightClickEvent e);
    void onGuiScrollViewEvent(ofxDatGuiScrollViewEvent e);
    
    void newModuleListener(ofxDatGuiDropdownEvent e);
    void newPresetListener(ofxDatGuiTextInputEvent e);
    
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
    
    void savePreset(string presetName, string bank);
    void loadPreset(string presetName, string bank);
    void loadPresetWithFade(int presetNum, string bank);
    
    void changePresetLabelHighliht(ofxDatGuiButton* presetToHighlight);
    
    void saveGuiArrangement();
    void loadGuiArrangement();
    
    bool loadPresetsSequence();
    
    void loadPresetWhenFadeOutCompletes(float *arg);
    
    void loadBank();
    
    void setGlobalBPM(float bpm);
    
    void destroyedConnection(ofAbstractParameter &disconnectedParameter);
    
    ofEvent<pair<moduleType, ofPoint>>  createNewModule;
    ofEvent<string>                     destroyModule;
    ofEvent<void>                       nextFrameEvent;
private:
    
    void setFromNormalizedValue(ofAbstractParameter* p, float v);
    void setFromSameTypeValue(shared_ptr<nodeConnection> connection);
    
    void destroyModuleAndConnections(int index);
    
    ofxDatGui *datGui;
    ofxDatGuiMatrix* presetMatrix;
    ofxDatGuiDropdown* bankSelect;
    ofxDatGuiScrollView* presetsList;
    
    vector<shared_ptr<ofxDatGui>> datGuis;
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
    
    //BPM Detect
    bpmControl *beatTracker;
    float       newBpm;
    
    
    //node
    vector<shared_ptr<nodeConnection>>  connections;
    
    bool    commandPressed = false;
    bool    canvasDragging = false;
    
    ofMatrix4x4 transformMatrix;
    ofPoint dragCanvasInitialPoint;
    
    
    ofxDatGuiTheme*     mainGuiTheme;
    ofxDatGuiButton*    oldPresetButton;
    
    //CURSORS
    GLFWcursor* arrowCursor;
    GLFWcursor* arrowListCursor;
    GLFWcursor* openedHandCursor;
    GLFWcursor* closedHandCursor;
    GLFWcursor* crossCursor;
    GLFWcursor* zoomInCursor;
    GLFWcursor* zoomOutCursor;
};


#endif /* parametersControl_h */
