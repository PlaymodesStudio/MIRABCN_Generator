//
//  waveScope.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 10/01/2017.
//
//

#include "waveScope.h"

waveScope::waveScope(){
    
    oscillatorBankIns.resize(2);
    
    parameters = new ofParameterGroup();
    parameters->setName("WaveScope");
    parameters->add(mainOutIn.set("Master Scope", {{0}}));
    parameters->add(oscillatorBankIns[0].set("Osc Bank 1", {0}));
    parameters->add(oscillatorBankIns[1].set("Osc Bank 2", {0}));

    parametersControl::getInstance().createGuiFromParams(parameters);
}

void waveScope::draw(){
    ofBackground(0);
    ofSetColor(255);
    int contentWidth = 2*ofGetWidth()/3;
//    //Draw the fbo
//    pixelContent.getTexture().draw(0, 0, contentWidth, ofGetHeight()/3);
//    ofPushStyle();
//    ofSetColor(ofColor::indianRed);
//    ofNoFill();
//    ofSetLineWidth(2);
//    ofDrawRectangle(0, 0, contentWidth, ofGetHeight()/3);
//    ofPopStyle();
//    
//    waveGrid.getTexture().draw(contentWidth, 0, ofGetWidth()-contentWidth, ofGetHeight()/3);
//    ofPushStyle();
//    ofSetColor(ofColor::indianRed);
//    ofNoFill();
//    ofSetLineWidth(2);
//    ofDrawRectangle(contentWidth, 0, ofGetWidth()-contentWidth, ofGetHeight()/3);
//    ofPopStyle();
    
    //Draw the Bars
    int numBars = oscillatorBankIns[0].get().size();
    float wid = (float)contentWidth/numBars;
    float hei = ofGetHeight()/3;
    for(int i = 0; i < numBars; i++)
        ofDrawRectangle((i*wid), (1-oscillatorBankIns[0].get()[i])*hei+hei, wid, oscillatorBankIns[0].get()[i]*hei);
    ofPushStyle();
    ofSetColor(ofColor::indianRed);
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawRectangle(0, ofGetHeight()/3, contentWidth, ofGetHeight()/3);
    ofPopStyle();
    
    
//    //Draw the Bars2
//    waveLinear.getTexture().draw(0, 2*ofGetHeight()/3, contentWidth, ofGetHeight()/3);
//    ofPushStyle();
//    ofSetColor(ofColor::indianRed);
//    ofNoFill();
//    ofSetLineWidth(2);
//    ofDrawRectangle(0, 2*ofGetHeight()/3, contentWidth, ofGetHeight()/3);
//    ofPopStyle();
//    
//    //Draw another time the grid
//    //waveGrid.getTexture().draw(contentWidth, 2*ofGetHeight()/3, ofGetWidth()-contentWidth, ofGetHeight()/3);
//    
//    
//    //Draw notifiers
//    ofRectangle debugRectangle(contentWidth, ofGetHeight()/3, ofGetWidth()-contentWidth, 2*ofGetHeight()/3);
//    
//    while(logBuffer->getSize()*15 > 2*ofGetHeight()/3) logBuffer->eraseLastLine();
//    
//    for (int i = 0; i < logBuffer->getSize(); i++){
//        string line = logBuffer->getLine(i);
//        ofDrawBitmapString(line, debugRectangle.x, debugRectangle.y + (15*(i+1)));
//    }
//    
//    
//    //Draw the framerate
//    ofSetColor(255, 0,0);
//    ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, ofGetHeight()-10);
}

ofFbo waveScope::computeLinWaveTex(vector<float> values){
    //Draw info to the FBO's
    ofFbo tex;
    tex.begin();
    
    //Draw the Bars
    float hei = tex.getHeight();
    //    ofSetColor(0);
    //    ofDrawRectangle(index, hei, 1, hei);
    
    ofSetColor(255);
    for(int i = 0; i<values.size(); i++)
        ofDrawRectangle(i, (1-values[i])*hei, 1, values[i]*hei);
    
    tex.end();
    
    return tex;
}
