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
    parameters->setName("waveScope");
    parameters->add(mainOutIn.set("Master Scope", {{0}}));
    for(int i = 0; i < numBankScopes ; i++)
        parameters->add(oscillatorBankIns[i].set("Osc Bank "+ ofToString(i), {0}));
    parameters->add(drawOnSeparateWindow.set("Separate Window", false));

    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::white, pos);
    
    contentWidthOffset = 0;
    
    drawOnSeparateWindow.addListener(this, &waveScope::changeDrawLocation);
    
    prevWindowRect.setPosition(-1, -1);
    
}

void waveScope::draw(){
    ofBackground(0);
    ofSetColor(255);
    
    int contentWidth = 2*ofGetWidth()/3 + contentWidthOffset;
    
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
    //TODO: Here will be better not to erase the messages and use mouseScrollData to get the older messages.
    
    int lineHeigh = 15;
    
//    ofRectangle debugRectangle(contentWidth, ofGetHeight()/3, ofGetWidth()-contentWidth, 2*ofGetHeight()/3);
    ofRectangle debugRectangle(contentWidth, 0, ofGetWidth()-contentWidth, ofGetHeight());
    while(logBuffer->getSize()*lineHeigh > debugRectangle.getHeight()) logBuffer->eraseLastLine();

    for (int i = 0; i < logBuffer->getSize(); i++){
        string line = logBuffer->getLine(i);
        ofDrawBitmapString(line, debugRectangle.x, debugRectangle.y + (lineHeigh*(i+1)));
    }
    
    
    //Draw the framerate
    ofSetColor(255, 0,0);
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, ofGetHeight()-10);
}



void waveScope::mouseMoved(ofMouseEventArgs &a){
    if(abs(a.x - (2*ofGetWidth()/3 + contentWidthOffset)) < 10){
        if(!isInMovableRegion){
            isInMovableRegion = true;
            GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
            glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), cursor);
        }
    }
    else if(isInMovableRegion){
        isInMovableRegion = false;
        GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        glfwSetCursor((GLFWwindow*)ofGetWindowPtr()->getWindowContext(), cursor);
    }
}

void waveScope::mousePressed(ofMouseEventArgs &a){
    mousePressInititalX = a.x;
    
    
}

void waveScope::mouseReleased(ofMouseEventArgs &a){
    
}

void waveScope::mouseDragged(ofMouseEventArgs &a){
    if(isInMovableRegion){
        contentWidthOffset = contentWidthOffset + a.x - mousePressInititalX;
        mousePressInititalX = a.x;
    }
}


void waveScope::changeDrawLocation(bool &b){
    if(b){
        ofGLFWWindowSettings prevSettings;
        if(prevWindowRect.getPosition() == ofPoint(-1, -1)){
            prevSettings.width = 300;
            prevSettings.height = 250;
            prevSettings.setPosition(ofVec2f(ofGetScreenWidth()-300, 0));
        }
        else{
            prevSettings.width = prevWindowRect.width;
            prevSettings.height = prevWindowRect.height;
            prevSettings.setPosition(prevWindowRect.position);
        }
        prevSettings.windowMode = OF_WINDOW;
        prevSettings.resizable = true;
        prevWindow = ofCreateWindow(prevSettings);
        ofAddListener(prevWindow->events().draw, this, &waveScope::drawEvent);
        ofAddListener(prevWindow->events().mouseMoved, this, &waveScope::mouseMoved);
        ofAddListener(prevWindow->events().mousePressed, this, &waveScope::mousePressed);
        ofAddListener(prevWindow->events().mouseReleased, this, &waveScope::mouseReleased);
        ofAddListener(prevWindow->events().mouseDragged, this, &waveScope::mouseDragged);
    }
    else{
        prevWindowRect.setPosition(prevWindow->getWindowPosition());
        prevWindowRect.setSize(prevWindow->getWidth(), prevWindow->getHeight());
        prevWindow->setWindowShouldClose();
        prevWindow = nullptr;
    }
}
