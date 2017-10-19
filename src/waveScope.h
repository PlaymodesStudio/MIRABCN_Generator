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
    waveScope(shared_ptr<bufferLoggerChannel> logBuffer_ = nullptr, int groupScopes = 1, int colorScopes = 1, int bankScopes = 2, bool gridScope = false, ofPoint pos = ofPoint(-1, -1));
    ~waveScope(){
        delete parameters;
        if(prevWindow != nullptr)
            prevWindow->setWindowShouldClose();
    };
    
    void activateSeparateWindow(ofRectangle winRect = ofRectangle(-1, -1, 100, 100));
    shared_ptr<ofAppBaseWindow> getWindow(){return prevWindow;};
    
    void draw();
    void drawEvent(ofEventArgs &e){draw();};
    
    void mouseMoved(ofMouseEventArgs &a);
    void mousePressed(ofMouseEventArgs &a);
    void mouseReleased(ofMouseEventArgs &a);
    void mouseDragged(ofMouseEventArgs &a);
private:
    
    void changeDrawLocation(bool &b);
    
    void inputListener(ofAbstractParameter &abs);
    
    ofParameterGroup* parameters;
    vector<ofParameter<vector<float>>>          oscillatorBankIns;
    vector<ofParameter<vector<vector<float>>>>  groupBankIn;
    vector<ofParameter<vector<vector<ofColor>>>>    colorGroupIn;
    ofParameter<vector<float>>  gridIn;
    
    vector<float>   activeGroupInCounter;
    vector<float>   activeColorInCounter;
    vector<float>   activeOscInCounter;
    float   activeGridCounter;
    
    ofParameter<vector<vector<float>>> mainOutIn;
    ofParameter<vector<vector<ofColor>>> gradientPreview;
    ofParameter<vector<vector<ofColor>>> colorTexture;
    ofParameter<bool>   drawOnSeparateWindow;
    
    shared_ptr<bufferLoggerChannel> logBuffer;
    
    int contentWidthOffset;
    int mousePressInititalX;
    bool isInMovableRegion;
    bool hasColor;
    
    bool manualWindowCreation = false;
    
    shared_ptr<ofAppBaseWindow> prevWindow;
    ofRectangle                 prevWindowRect;
};

#endif /* waveScope_h */
