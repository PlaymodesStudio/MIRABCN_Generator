#pragma once

#include "ofMain.h"
#include "phasorClass.h"
#include "ofxSyphon.h"
//#include "elementOscilator.h"
//#include "Wave2DControl.h"
#include "parametersControl.h"
#include "ofxCurvesTool.h"
#include "bufferLoggerChannel.h"
#include "masterControl.h"
#include "colorApplier.h"
#include "senderManager.h"
#include "oscillatorBank.h"
#include "waveScope.h"
#include "oscillatorBankGroup.h"
#include "sharedResources.h"

static const int NUM_BARS = 12;
static const int PIXEL_X_BAR = 20;
static const int ROW_BARS = 4;
static const int COL_BARS = 3;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    void exit();
    
    void drawSecondWindow(ofEventArgs & args);
    void keyPressedOnSecondWindow(ofKeyEventArgs & args);
    void audioIn(float * input, int bufferSize, int nChannels);


		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void setFile(string path){fileName = path;};
    
private:
    
    void newModuleListener(pair<moduleType, ofPoint> &info);
    void deleteModuleListener(string &moduleName);


    
    //If you open a project file, can be a .generator file, has the following clases for all presets:
    // - 1 PhasorClass
    // - 1 OscillatorBankGroup with the size as parameter, probably wont need more than one in each project
    // - 1 senderManager module (from parameters set syphon names and osc ports
    // - 1 waveScope (from parameters set number of oscillatorBank scopes)
    vector<shared_ptr<phasorClass>>        phasors;
    vector<shared_ptr<oscillatorBank>>     oscillators;
    vector<shared_ptr<baseOscillator>>     monoOscillator;
//    Wave2DControl   waveControl;
    parametersControl*          paramsControl;
    senderManager*              senderModule;
    waveScope*                  preview;
    colorApplier*               colorModule;
    oscillatorBankGroup*        oscBankGroup;
    
    ofSoundStream soundStream;
    
    string fileName = " ";
    
    //The log channel is a buffer where you post all notifications, it's used mainly in waveScope to print the debug info.
    shared_ptr<bufferLoggerChannel> logBuffer;
};
