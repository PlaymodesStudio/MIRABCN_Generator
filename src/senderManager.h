//
//  senderManager.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 01/12/2016.
//
//

#ifndef senderManager_h
#define senderManager_h

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxOsc.h"
#include "parametersControl.h"

class senderManager{
public:
    
    senderManager();
    ~senderManager(){};
    
    void send(vector<vector<float>> &greyscaleInfo, vector<vector<ofColor>> &colorInfo, ofFbo &grayscaleFbo, ofFbo &colorFbo){};
    void send(ofFbo &grayscaleFbo, ofFbo &colorFbo);
    void send(ofFbo &grayscaleFbo){};
    void send(vector<vector<float>> &greyscaleInfo, vector<vector<ofColor>> &colorInfo);
    void send(vector<vector<float>> &greyscaleInfo);
    
    //listeres
    void enableOscListener(bool &b);
    void enableSyphonListener(bool &b);
    
    void oscHostOrOscPortListener(string &s);
    void syphonNameListener(string &s);
    
private:
    ofxSyphonServer*    grayscaleSyphonServer;
    ofxSyphonServer*    colorSyphonServer;
    
    ofxOscSender*       oscSender;
    
    ofParameterGroup    parameters;
    ofParameter<bool>   enableOsc;
    ofParameter<bool>   enableSyphon;
    
    ofParameter<string> grayscaleSyphonName;
    ofParameter<string> colorSyphonName;
    
    ofParameter<string> oscHost;
    ofParameter<string> oscPort;
};

#endif /* senderManager_h */
