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
    
    senderManager(int _id, bool _invert = false, string _grayscaleName = "Gen_Grayscale", string _colorName = "Gen_Color");
    ~senderManager(){};
    
    void send(vector<vector<float>> &greyscaleInfo, vector<vector<ofColor>> &colorInfo, ofFbo &grayscaleFbo, ofFbo &colorFbo){};
    void send(ofFbo &grayscaleFbo, ofFbo &colorFbo);
    void send(ofFbo &grayscaleFbo){};
    void send(vector<vector<float>> &greyscaleInfo, vector<vector<ofColor>> &colorInfo);
    void send(vector<vector<float>> &greyscaleInfo);
    
    void setGrayName(string name){grayscaleSyphonName = name;};
    void setColorName(string name){colorSyphonName = name;};
    
    //listeres
    void enableOscListener(bool &b);
    void enableSyphonListener(bool &b);
    
    void oscHostOrOscPortListener(string &s);
    void syphonNameListener(string &s);
    
private:
    void sendGrayScale(vector<vector<float>> &info);
    void sendColor(vector<vector<ofColor>> &info);
    
    ofxSyphonServer*    grayscaleSyphonServer;
    ofxSyphonServer*    colorSyphonServer;
    
    ofxOscSender*       oscSender;
    
    ofParameterGroup*    parameters;
    ofParameter<bool>   enableOsc;
    ofParameter<bool>   enableSyphon;
    
    ofParameter<string> grayscaleSyphonName;
    ofParameter<string> colorSyphonName;
    
    ofParameter<string> oscHost;
    ofParameter<string> oscPort;
    
    ofParameter<float>  masterFader;
    
    ofParameter<vector<vector<float>>>  grayScaleIn;
    ofParameter<vector<vector<ofColor>>>    colorIn;
    
    bool invert;
    
    ofFbo colorTexToSend;
    ofFbo grayScaleTexToSend;
};

#endif /* senderManager_h */
