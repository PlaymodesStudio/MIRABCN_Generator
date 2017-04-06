//
//  senderManager.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 01/12/2016.
//
//

#include "senderManager.h"

senderManager::senderManager(){
    parameters = new ofParameterGroup;
    parameters->setName("senderManager");
    parameters->add(enableSyphon.set("Enable Syphon", 0));
    parameters->add(grayscaleSyphonName.set("Grayscale Server Name", "Gen_Grayscale"));
    parameters->add(colorSyphonName.set("Color Server Name", "Gen_Color"));

    parameters->add(enableOsc.set("Enable OSC", 0));
    parameters->add(oscHost.set("Host", "127.0.0.1"));
    parameters->add(oscPort.set("Port", "1234"));
    
    parameters->add(grayScaleIn.set("Grayscale In", {{0}}));
    parameters->add(colorIn.set("Color In", {{ofColor::white}}));
    
    grayScaleIn.addListener(this, &senderManager::sendGrayScale);
    colorIn.addListener(this, &senderManager::sendColor);
    
    
                    
    
    parametersControl::getInstance().createGuiFromParams(parameters);
    
    enableOsc.addListener(this, &senderManager::enableOscListener);
    enableSyphon.addListener(this, &senderManager::enableSyphonListener);
}

void senderManager::sendGrayScale(vector<vector<float>> &info){
    int w = info.size();
    int h = info[0].size();
    if(enableOsc){
        unique_ptr<ofxOscMessage> messageGrayscale = unique_ptr<ofxOscMessage>(new ofxOscMessage());
        messageGrayscale->setAddress("size");
        messageGrayscale->addIntArg(w);
        messageGrayscale->addIntArg(h);
        oscSender->sendMessage(*messageGrayscale);
        messageGrayscale = unique_ptr<ofxOscMessage>(new ofxOscMessage());
        messageGrayscale->setAddress("info/grayscale");
        
        for(int i = 0 ; i < w ; i++){
            for (int j = 0 ; j < h ; j++){
                messageGrayscale->addFloatArg(info[i][j]);
            }
        }
        oscSender->sendMessage(*messageGrayscale);
    }
    if(grayscaleSyphonServer != NULL && enableSyphon){
        ofTexture tex;
        unsigned char *data = new unsigned char[w * h];
        for(int i = 0 ; i < w ; i++){
            for ( int j = 0; j < h ; j++)
                data[i+w*j] = info[i][j]*255;
        }
        tex.loadData(data, w, h, GL_LUMINANCE);
        grayscaleSyphonServer->publishTexture(&tex);
    }
}

void senderManager::sendColor(vector<vector<ofColor>> &info){
    int w = info.size();
    int h = info[0].size();
    if(enableOsc){
        ofxOscMessage* messageColor = new ofxOscMessage();
        messageColor->setAddress("info/color");
        
        for(int i = 0 ; i < w ; i++){
            for (int j = 0 ; j < h ; j++){
                messageColor->addFloatArg((float)info[i][j].r);
                messageColor->addFloatArg((float)info[i][j].g);
                messageColor->addFloatArg((float)info[i][j].b);
            }
        }
        oscSender->sendMessage(*messageColor);
    }
    if(colorSyphonServer != NULL && enableSyphon){
        ofTexture tex;
        unsigned char *data = new unsigned char[w * h*3];
        for(int i = 0 ; i < w ; i++){
            for ( int j = 0; j < h ; j++){
                data[(i*3)+w*3*j] = info[i][j].r;
                data[(i*3)+(w*3*j)+1] = info[i][j].g;
                data[(i*3)+(w*3*j)+2] = info[i][j].b;
            }
        }
        tex.loadData(data, w, h, GL_RGB);
        colorSyphonServer->publishTexture(&tex);
    }
}

void senderManager::send(ofFbo &grayscaleFbo, ofFbo &colorFbo){
    if(grayscaleSyphonServer != NULL && enableSyphon)
        grayscaleSyphonServer->publishTexture(&grayscaleFbo.getTexture());
    if(colorSyphonServer != NULL && enableSyphon)
        colorSyphonServer->publishTexture(&colorFbo.getTexture());
}

void senderManager::send(vector<vector<float>> &grayscaleInfo, vector<vector<ofColor>> &colorInfo){
    if(enableOsc){
        ofxOscMessage* messageGrayscale = new ofxOscMessage();
        messageGrayscale->setAddress("info/grayscale");
        
        ofxOscMessage* messageColor = new ofxOscMessage();
        messageColor->setAddress("info/color");
        
        for(int i = 0 ; i < grayscaleInfo.size() ; i++){
            for (int j = 0 ; j < grayscaleInfo[i].size() ; j++){
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

void senderManager::send( vector<vector<float>> &grayscaleInfo){
    if(enableOsc){
        ofxOscMessage messageGrayscale;
        messageGrayscale.setAddress("info/grayscale");
    
        
        for(int i = 0 ; i < grayscaleInfo.size() ; i++){
            for (int j = 0 ; j < grayscaleInfo[i].size() ; j++){
                messageGrayscale.addFloatArg(grayscaleInfo[i][j]);
            }
        }
        oscSender->sendMessage(messageGrayscale);
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
        grayscaleSyphonServer = new ofxSyphonServer;
        colorSyphonServer = new ofxSyphonServer;
        
        grayscaleSyphonServer->setName(grayscaleSyphonName);
        colorSyphonServer->setName(colorSyphonName);
        
        colorSyphonName.addListener(this, &senderManager::syphonNameListener);
        grayscaleSyphonName.addListener(this, &senderManager::syphonNameListener);
    }else{
        colorSyphonName.removeListener(this, &senderManager::syphonNameListener);
        grayscaleSyphonName.removeListener(this, &senderManager::syphonNameListener);
        
        delete grayscaleSyphonServer;
        delete colorSyphonServer;
    }
}

void senderManager::oscHostOrOscPortListener(string &s){
    oscSender->setup(oscHost, ofToInt(oscPort));
}

void senderManager::syphonNameListener(string &s){
    grayscaleSyphonServer->setName(grayscaleSyphonName);
    colorSyphonServer->setName(colorSyphonName);
}
