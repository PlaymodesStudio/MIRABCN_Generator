//
//  audioEngineController.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 26/07/2017.
//
//

#include "audioEngineController.h"
#include "parametersControl.h"

audioEngineController::audioEngineController(){
    parameters = new ofParameterGroup();
    parameters->setName("audioControls");
    parameters->add(oscHost.set("Osc Host", "localhost"));
    parameters->add(oscPort.set("Osc Port", "11511"));
    parameters->add(presetNum.set("Preset Num", 0, 0, 100));
    parameters->add(param_a.set("Param a", 0, 0, 1));
    parameters->add(param_b.set("Param b", 0, 0, 1));
    parameters->add(param_c.set("Param c", 0, 0, 1));
    parameters->add(param_va.set("Vector a", {0}));
    parameters->add(param_vb.set("Vector b", {0}));
    parameters->add(param_vc.set("Vector c", {0}));
    
    oscSender.setup(oscHost, ofToInt(oscPort));
    
    presetNum.addListener(this, &audioEngineController::presetNumSender);
    
    oscHost.addListener(this, &audioEngineController::oscHostAndPortListener);
    oscPort.addListener(this, &audioEngineController::oscHostAndPortListener);
    
    param_a.addListener(this, &audioEngineController::param_aSender);
    param_b.addListener(this, &audioEngineController::param_bSender);
    param_c.addListener(this, &audioEngineController::param_cSender);
    
    param_va.addListener(this, &audioEngineController::param_vaSender);
    param_vb.addListener(this, &audioEngineController::param_vbSender);
    param_vc.addListener(this, &audioEngineController::param_vcSender);
    
    parametersControl::getInstance().createGuiFromParams(parameters);
}

void audioEngineController::oscHostAndPortListener(string &s){
    oscSender.setup(oscHost, ofToInt(oscPort));
}

void audioEngineController::presetNumSender(int &i){
    ofxOscMessage message;
    message.setAddress("audio/preset");
    message.addIntArg(i);
    oscSender.sendMessage(message);
}

void audioEngineController::param_aSender(float &f){
    ofxOscMessage message;
    message.setAddress("audio/a");
    message.addFloatArg(f);
    oscSender.sendMessage(message);
}

void audioEngineController::param_bSender(float &f){
    ofxOscMessage message;
    message.setAddress("audio/b");
    message.addFloatArg(f);
    oscSender.sendMessage(message);
}

void audioEngineController::param_cSender(float &f){
    ofxOscMessage message;
    message.setAddress("audio/c");
    message.addFloatArg(f);
    oscSender.sendMessage(message);
}

void audioEngineController::param_vaSender(vector<float> &vf){
    ofxOscMessage message;
    message.setAddress("audio/va");
    for(auto f : vf){
        message.addFloatArg(f);
    }
    oscSender.sendMessage(message);
}

void audioEngineController::param_vbSender(vector<float> &vf){
    ofxOscMessage message;
    message.setAddress("audio/vb");
    for(auto f : vf){
        message.addFloatArg(f);
    }
    oscSender.sendMessage(message);
}

void audioEngineController::param_vcSender(vector<float> &vf){
    ofxOscMessage message;
    message.setAddress("audio/vc");
    for(auto f : vf){
        message.addFloatArg(f);
    }
    oscSender.sendMessage(message);
}

