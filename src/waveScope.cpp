//
//  waveScope.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 10/01/2017.
//
//

#include "waveScope.h"

waveScope::waveScope(shared_ptr<bufferLoggerChannel> logBuffer_, int numBankScopes, ofPoint pos){
    logBuffer = logBuffer_;
    
    oscillatorBankIns.resize(numBankScopes);
    
    parameters = new ofParameterGroup();
    parameters->setName("WaveScope");
    parameters->add(mainOutIn.set("Master Scope", {{0}}));
    for(int i = 0; i < numBankScopes ; i++)
        parameters->add(oscillatorBankIns[i].set("Osc Bank "+ofToString(i), {0}));

    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::white, pos);
}

void waveScope::draw(){
    ofBackground(0);
    ofSetColor(255);
    int contentWidth = 2*ofGetWidth()/3;
    
    ofFbo outTex;
    outTex.allocate(mainOutIn.get().size(), mainOutIn.get()[0].size());
    
    
    outTex.begin();
    for( int i = 0; i < mainOutIn.get().size(); i++){
        for(int j = 0; j < mainOutIn.get()[i].size(); j++){
            int valueInByte = mainOutIn.get()[i][j]*255;
            
            ofSetColor(valueInByte);
            ofDrawRectangle(i,j, 1, 1);
            
        }
    }
    outTex.end();
    
    //Draw the fbo
    outTex.getTexture().draw(0, 0, contentWidth, ofGetHeight()/3);
    ofPushStyle();
    ofSetColor(ofColor::indianRed);
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawRectangle(0, 0, contentWidth, ofGetHeight()/3);
    ofPopStyle();

    
    //Draw the Bars
    int elementHeight = (ofGetHeight()*2/3) / oscillatorBankIns.size();
    for(int i = 0; i < oscillatorBankIns.size(); i++){
        int topPosition = ofGetHeight()/3 + (elementHeight * i);
        int numBars = oscillatorBankIns[i].get().size();
        float wid = (float)contentWidth/numBars;
        for(int j = 0; j < numBars; j++)
            ofDrawRectangle((j*wid), (1-oscillatorBankIns[i].get()[j])*elementHeight+topPosition, wid, oscillatorBankIns[i].get()[j]*elementHeight);
        ofPushStyle();
        ofSetColor(ofColor::indianRed);
        ofNoFill();
        ofSetLineWidth(2);
        ofDrawRectangle(0, topPosition, contentWidth, elementHeight);
        ofPopStyle();
    }
    
    
    //Draw notifiers
    // Here will be better not to erase the messages and use mouseScrollData to get the older messages.
    ofRectangle debugRectangle(contentWidth, ofGetHeight()/3, ofGetWidth()-contentWidth, 2*ofGetHeight()/3);
    
    while(logBuffer->getSize()*15 > 2*ofGetHeight()/3) logBuffer->eraseLastLine();

    for (int i = 0; i < logBuffer->getSize(); i++){
        string line = logBuffer->getLine(i);
        ofDrawBitmapString(line, debugRectangle.x, debugRectangle.y + (15*(i+1)));
    }
    
    
    //Draw the framerate
    ofSetColor(255, 0,0);
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, ofGetHeight()-10);
}
