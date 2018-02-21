//
//  senderManager.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 01/12/2016.
//
//

#include "senderManager.h"

senderManager::senderManager(int _id, bool _invert, string _serverName){
    invert = _invert;
    parameters = new ofParameterGroup;
    parameters->setName("senderManager " + ofToString(_id));
    parameters->add(enableSyphon.set("Enable Syphon", 0));
    parameters->add(syphonName.set("Server Name", _serverName));

    parameters->add(enableOsc.set("Enable OSC", 0));
    parameters->add(oscHost.set("Host", "127.0.0.1"));
    parameters->add(oscPort.set("Port", "1234"));
    
    parameters->add(masterFader.set("Master Fader", 1, 0, 1));
    
    parameters->add(textureIn.set("Texture In", nullptr));
    
    textureIn.addListener(this, &senderManager::sendColor);
    
    
                    
    
    parametersControl::getInstance().createGuiFromParams(parameters);
    
    enableOsc.addListener(this, &senderManager::enableOscListener);
    enableSyphon.addListener(this, &senderManager::enableSyphonListener);
    
    enableSyphon = true;
}


void senderManager::sendColor(ofTexture *&info){
//    int w = info.size();
//    int h = info[0].size();
//    if(enableOsc){
//        ofxOscMessage* messageColor = new ofxOscMessage();
//        messageColor->setAddress("info/color");
//        
//        for(int i = 0 ; i < w ; i++){
//            for (int j = 0 ; j < h ; j++){
//                messageColor->addFloatArg((float)info[i][j].r*masterFader);
//                messageColor->addFloatArg((float)info[i][j].g*masterFader);
//                messageColor->addFloatArg((float)info[i][j].b*masterFader);
//            }
//        }
//        oscSender->sendMessage(*messageColor);
//        delete messageColor;
//    }
    if(syphonServer != NULL && enableSyphon){
        syphonServer->publishTexture(info);
    }
}


#pragma mark -- Listeners --
void senderManager::enableOscListener(bool &b){
    if(b){
        oscSender = new ofxOscSender();
        oscSender->setup(oscHost, ofToInt(oscPort));
        oscHost.addListener(this, &senderManager::oscHostOrOscPortListener);
        oscPort.addListener(this, &senderManager::oscHostOrOscPortListener);
    }else{
        oscHost.removeListener(this, &senderManager::oscHostOrOscPortListener);
        oscPort.removeListener(this, &senderManager::oscHostOrOscPortListener);
        delete oscSender;
    }
}

void senderManager::enableSyphonListener(bool &b){
    if(b){
        syphonServer = new ofxSyphonServer;
        
        syphonServer->setName(syphonName);
        
        syphonName.addListener(this, &senderManager::syphonNameListener);
    }else{
        syphonName.removeListener(this, &senderManager::syphonNameListener);
        delete syphonServer;
    }
}

void senderManager::oscHostOrOscPortListener(string &s){
    oscSender->setup(oscHost, ofToInt(oscPort));
}

void senderManager::syphonNameListener(string &s){
    syphonServer->setName(syphonName);
}
