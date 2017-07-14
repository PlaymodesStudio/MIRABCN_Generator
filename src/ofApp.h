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
#include "envelopeGenerator.h"
#include "typeConverter.h"
#include "expressionOperator.h"
#include "midiGateIn.h"


static const int WIDTH = 0;
static const int HEIGHT = 0;
static const int ROW_BARS = 4;
static const int COL_BARS = 3;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    void exit();
    
    void drawSecondWindow(ofEventArgs & args);
    void mouseMovedSecondWindow(ofMouseEventArgs &a);
    void mouseDraggedSecondWindow(ofMouseEventArgs &a);
    void mousePressedSecondWindow(ofMouseEventArgs &a);
    void mouseReleasedSecondWindow(ofMouseEventArgs &a);
    
    
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
    
    void nextFrameListener();
    
    
private:
    
    void newModuleListener(pair<string, ofPoint> &info);
    void deleteModuleListener(string &moduleName);

    int width;
    int height;
    int hasColorApplier;
    
    //If you open a project file, can be a .generator file, has the following clases for all presets:
    // - 1 PhasorClass
    // - 1 OscillatorBankGroup with the size as parameter, probably wont need more than one in each project
    // - 1 senderManager module (from parameters set syphon names and osc ports
    // - 1 waveScope (from parameters set number of oscillatorBank scopes)
    vector<phasorClass*>        phasors;
    vector<baseOscillator*> oscillators;
    vector<oscillatorBank*> oscillatorBanks;
    vector<envelopeGenerator*>  envelopeGens;
    vector<senderManager*>      senderModules;
    vector<typeConverter<vector<float>, vector<vector<float>>>*> converters;
    vector<abstractExpressionOperator*> expressionOps;
//    Wave2DControl   waveControl;
    parametersControl*          paramsControl;
    senderManager*              senderModule;
    waveScope*                  preview;
    vector<colorApplier*>               colorModules;
    vector<oscillatorBankGroup*>       oscBankGroups;
    
    ofSoundStream soundStream;
    
    //The log channel is a buffer where you post all notifications, it's used mainly in waveScope to print the debug info.
    shared_ptr<bufferLoggerChannel> logBuffer;
};
