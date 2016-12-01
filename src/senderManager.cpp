//
//  senderManager.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 01/12/2016.
//
//

#include "senderManager.h"

senderManager::senderManager(){
    
    parameters.setName("senderManager " + ofToString(1));
    parameters.add(enableSyphon.set("Enable Syphon", 0));
    parameters.add(grayscaleSyphonName.set("Grayscale Server Name", "Gen_Grayscale"));
    parameters.add(colorSyphonName.set("Color Server Name", "Gen_Color"));
    parameters.add(enableOsc.set("Enable OSC", 0));
    parameters.add(oscHost.set("Host", "127.0.0.1"));
    parameters.add(oscPort.set("Port", "1234"));
    
    
    enableOsc.addListener(this, &senderManager::enableOscListener);
    enableSyphon.addListener(this, &senderManager::enableSyphonListener);
}

void senderManager::send( vector<vector<float>> &grayscaleInfo, vector<vector<ofColor>> &colorInfo){
    if(enableOsc){
        ofxOscMessage* messageGrayscale = new ofxOscMessage();
        messageGrayscale->setAddress("info/grayscale");
        
        ofxOscMessage* messageColor = new ofxOscMessage();
        messageColor->setAddress("info/color");
        
        for(int i = 0 ; i < colorInfo.size() ; i++){
            for (int j = 0 ; j < colorInfo[i].size() ; j++){
                messageGrayscale->addFloatArg(grayscaleInfo[i][j]);
                messageColor->addFloatArg((float)colorInfo[i][j].r);
                messageColor->addFloatArg((float)colorInfo[i][j].g);
                messageColor->addFloatArg((float)colorInfo[i][j].b);
            }
        }
        oscSender->sendMessage(*messageGrayscale);
        oscSender->sendMessage(*messageColor);
    }
}

#pragma mark -- Listeners --
void senderManager::enableOscListener(bool &b){
    if(b){
        oscSender = new ofxOscSender();
        oscSender->setup(oscHost, ofToInt(oscPort));
    }else{
        delete oscSender;
    }
}

void senderManager::enableSyphonListener(bool &b){
    if(b){
        grayscaleSyphonServer = new ofxSyphonServer;
        colorSyphonServer = new ofxSyphonServer;
        
        grayscaleSyphonServer->setName(grayscaleSyphonName);
        colorSyphonServer->setName(colorSyphonName);
    }else{
        delete grayscaleSyphonServer;
        delete colorSyphonServer;
    }
}
