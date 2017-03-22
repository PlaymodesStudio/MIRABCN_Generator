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
    
    
private:
    
    void newModuleListener(pair<moduleType, ofPoint> &info);


    vector<phasorClass*>        phasors;
    vector<oscillatorBank*>     oscillators;
    vector<baseOscillator*>     monoOscillator;
//    Wave2DControl   waveControl;
    parametersControl*          paramsControl;
    masterControl               masterModule;
    senderManager*              senderModule;
    waveScope*                  preview;
    colorApplier*               colorModule;
    vector<oscillatorBankGroup*> oscBankGroup;
    
    ofSoundStream soundStream;
    
    shared_ptr<bufferLoggerChannel> logBuffer;
    
};
