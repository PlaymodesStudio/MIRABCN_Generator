#pragma once

#include "ofMain.h"
#include "phasorClass.h"
#include "ofxSyphon.h"
#include "elementOscilator.h"
#include "Wave2DControl.h"
#include "delayControl.h"
#include "parametersControl.h"

static const int NUM_BARS = 24;
static const int PIXEL_X_BAR = 162;
static const int ROW_BARS = 4;
static const int COL_BARS = 3;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
    void drawSecondWindow(ofEventArgs & args);

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
    int pixelNum;
    
    vector<float> infoVec;
    ofFbo pixelContent;
    ofFbo waveGrid;
    
    
    ofxSyphonServer syphonServer;
    
    phasorClass phasor;
    elementOscilator singleGenerator;
    delayControl     delayControler;
    Wave2DControl   waveControl;
    
    parametersControl paramsControl;
    
    int guiWidth;
		
};
