//
//  waveScope.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 10/01/2017.
//
//

#include "waveScope.h"

waveScope::waveScope(shared_ptr<bufferLoggerChannel> logBuffer_, bool color, int numBankScopes, ofPoint pos){
    logBuffer = logBuffer_;
    hasColor = color;
    oscillatorBankIns.resize(numBankScopes);
    
    parameters = new ofParameterGroup();
    parameters->setName("waveScope");
    parameters->add(mainOutIn.set("Master Scope", {{0}}));
    if(color){
        parameters->add(gradientPreview.set("Color Scope", {{ofColor::black}}));
        parameters->add(colorTexture.set("Color Tex", {{ofColor::black}}));
    }
    for(int i = 0; i < numBankScopes ; i++)
        parameters->add(oscillatorBankIns[i].set("Osc Bank "+ ofToString(i), {}));
    parameters->add(drawOnSeparateWindow.set("Separate Window", false));

    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::white, pos);
    
    contentWidthOffset = 0;
    
    drawOnSeparateWindow.addListener(this, &waveScope::changeDrawLocation);
    
    prevWindowRect.setPosition(-1, -1);
    
}

void waveScope::draw(){
    ofBackground(0);
    ofSetColor(255);
//
    int contentWidth = 2*ofGetWidth()/3 + contentWidthOffset;
//
    int masterHeight = 2*ofGetHeight()/3;

    int w = mainOutIn.get().size();
    int h = mainOutIn.get()[0].size();
    
    ofTexture tex;
    tex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    unsigned char *data = new unsigned char[w * h];
    for(int i = 0 ; i < w ; i++){
        for ( int j = 0; j < h ; j++)
            data[i+w*j] = mainOutIn.get()[i][j]*255;
    }
    tex.loadData(data, w, h, GL_LUMINANCE);
    delete[] data;
    tex.draw(0,0,contentWidth, hasColor ? masterHeight/3 : masterHeight);
    ofPushStyle();
    ofSetColor(ofColor::indianRed);
    ofNoFill();
    ofSetLineWidth(2);
    ofDrawRectangle(0, 0, contentWidth, hasColor ? masterHeight/3 : masterHeight);
    ofPopStyle();

    if(hasColor){
        w = gradientPreview.get().size();
        h = gradientPreview.get()[0].size();
        ofTexture tex2;
        tex2.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        unsigned char *colordata = new unsigned char[w * h*3];
        for(int i = 0 ; i < w ; i++){
            for ( int j = 0; j < h ; j++){
                colordata[(i*3)+w*3*j] = gradientPreview.get()[i][j].r;
                colordata[(i*3)+(w*3*j)+1] = gradientPreview.get()[i][j].g;
                colordata[(i*3)+(w*3*j)+2] = gradientPreview.get()[i][j].b;
            }
        }
        tex2.loadData(colordata, w, h, GL_RGB);
        delete[] colordata;
        tex2.draw(0, masterHeight/3, contentWidth, masterHeight/3);
        ofPushStyle();
        ofSetColor(ofColor::indianRed);
        ofNoFill();
        ofSetLineWidth(2);
        ofDrawRectangle(0, masterHeight/3, contentWidth, masterHeight/3);
        ofPopStyle();
        
        
        w = colorTexture.get().size();
        h = colorTexture.get()[0].size();
        ofTexture tex3;
        tex3.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        unsigned char *color2data = new unsigned char[w * h*3];
        for(int i = 0 ; i < w ; i++){
            for ( int j = 0; j < h ; j++){
                color2data[(i*3)+w*3*j] = colorTexture.get()[i][j].r;
                color2data[(i*3)+(w*3*j)+1] = colorTexture.get()[i][j].g;
                color2data[(i*3)+(w*3*j)+2] = colorTexture.get()[i][j].b;
            }
        }
        tex3.loadData(color2data, w, h, GL_RGB);
        delete[] color2data;
        tex3.draw(0, 2*masterHeight/3, contentWidth, masterHeight/3);
        ofPushStyle();
        ofSetColor(ofColor::indianRed);
        ofNoFill();
        ofSetLineWidth(2);
        ofDrawRectangle(0, 2*masterHeight/3, contentWidth, masterHeight/3);
        ofPopStyle();
    }
    
    

    int numActiveOscBanks = 0;
    for(auto in : oscillatorBankIns)
        if(in.get().size() > 0) numActiveOscBanks++;
    
    //Draw the Bars
    if(numActiveOscBanks > 0){
        int elementHeight = (ofGetHeight() - masterHeight) / numActiveOscBanks;
        for(int i = 0; i < numActiveOscBanks; i++){
            int topPosition = masterHeight + (elementHeight * i);
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

void waveScope::activateSeparateWindow(ofRectangle winRect){
    prevWindowRect = winRect;
    if(!drawOnSeparateWindow) drawOnSeparateWindow = true;
}


void waveScope::changeDrawLocation(bool &b){
    if(b){
        ofAppBaseWindow* mainWindow = ofGetWindowPtr();
        
        ofGLFWWindowSettings prevSettings;
        if(prevWindowRect.getPosition() == ofPoint(-1, -1)){
            prevSettings.width = 300;
            prevSettings.height = 1000;
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
