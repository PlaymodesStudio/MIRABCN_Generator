//
//  waveScope.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 10/01/2017.
//
//

#include "waveScope.h"

void window_no_close_scope(GLFWwindow* window){
    glfwSetWindowShouldClose(window, GL_FALSE);
};


waveScope::waveScope(shared_ptr<bufferLoggerChannel> logBuffer_, int groupScopes, int colorScopes, int bankScopes, bool gridScope, ofPoint pos){
    logBuffer = logBuffer_;
    hasColor = colorScopes != 0;
    groupBankIn.resize(groupScopes);
    colorGroupIn.resize(colorScopes);
    oscillatorBankIns.resize(bankScopes);
    
    activeGroupInCounter.resize(groupScopes, 0);
    activeColorInCounter.resize(colorScopes, 0);
    activeOscInCounter.resize(bankScopes, 0);
    activeGridCounter = 0;
    
    parameters = new ofParameterGroup();
    parameters->setName("waveScope");
    for(int i = 0; i < groupScopes; i++){
        parameters->add(groupBankIn[i].set("Group In " + ofToString(i), nullptr));
    }
    for(int i = 0; i < colorScopes; i++){
        parameters->add(colorGroupIn[i].set("Color Group In " + ofToString(i), {{}}));
    }
    //    parameters->add(mainOutIn.set("Master Scope", {{0}}));
    //    if(color){
    //        parameters->add(gradientPreview.set("Color Scope", {{ofColor::black}}));
    //        parameters->add(colorTexture.set("Color Tex", {{ofColor::black}}));
    //    }
    for(int i = 0; i < bankScopes ; i++)
        parameters->add(oscillatorBankIns[i].set("Osc Bank "+ ofToString(i), {0}, {0},{1}));
    parameters->add(drawOnSeparateWindow.set("Separate Window", false));
    
    if(gridScope) {
        parameters->add(gridIn.set("Grid In", {0}));
    }
    
    ofAddListener(parameters->parameterChangedE(), this, &waveScope::inputListener);
    
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
    
    
    int numActiveGroups = 0;
    for(auto &in : activeGroupInCounter){
        if(in > 0){
            numActiveGroups++;
            in = in-1;
        }
    }
    
    int numActiveColors = 0;
    for(auto &in : activeColorInCounter){
        if(in > 0){
            numActiveColors++;
            in = in-1;
        }
    }
    
    int numActiveOscBanks = 0;
    for(auto &in : activeOscInCounter){
        if(in > 0){
            numActiveOscBanks++;
            in = in-1;
        }
    }
    if (activeGridCounter > 0) {
        activeGridCounter -=1;
    }
    
    int totalActiveScopes = numActiveGroups + numActiveColors + numActiveOscBanks;
    if(totalActiveScopes > 0){
        int elementHeight = ofGetHeight() / totalActiveScopes;
        int oscDrawIndex = 0;
        
        //Draw the groups
        for(int i = 0; i < groupBankIn.size(); i++){
            if(activeGroupInCounter[i] > 0 && groupBankIn[i] != nullptr){
//                int w = groupBankIn[i].get().size();
//                int h = groupBankIn[i].get()[0].size();
//                
//                ofTexture tex;
//                tex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
//                unsigned char *data = new unsigned char[w * h];
//                for(int ii = 0 ; ii < w ; ii++){
//                    for ( int jj = 0; jj < h ; jj++)
//                        data[ii+w*jj] = groupBankIn[i].get()[ii][jj]*255;
//                }
//                tex.loadData(data, w, h, GL_LUMINANCE);
//                delete[] data;
//                
                int topPosition = (elementHeight * oscDrawIndex);
                groupBankIn[i].get()->draw(0,topPosition, contentWidth, elementHeight);
                ofPushStyle();
                ofSetColor(ofColor::indianRed);
                ofNoFill();
                ofSetLineWidth(2);
                ofDrawRectangle(0, topPosition, contentWidth, elementHeight);
                ofPopStyle();
                oscDrawIndex++;
            }
        }
        
        //Draw the Color Groups
        for(int i = 0; i < colorGroupIn.size(); i++){
            if(activeColorInCounter[i] > 0){
                int w = colorGroupIn[i].get().size();
                int h = colorGroupIn[i].get()[0].size();
                
                ofTexture tex;
                tex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
                unsigned char *data = new unsigned char[w * h * 3];
                for(int ii = 0 ; ii < w ; ii++){
                    for ( int jj = 0; jj < h ; jj++){
                        data[(ii*3)+w*3*jj] = colorGroupIn[i].get()[ii][jj].r;
                        data[(ii*3)+(w*3*jj)+1] = colorGroupIn[i].get()[ii][jj].g;
                        data[(ii*3)+(w*3*jj)+2] = colorGroupIn[i].get()[ii][jj].b;
                    }
                }
                tex.loadData(data, w, h, GL_RGB);
                delete[] data;
                
                int topPosition = (elementHeight * oscDrawIndex);
                tex.draw(0,topPosition, contentWidth, elementHeight);
                ofPushStyle();
                ofSetColor(ofColor::indianRed);
                ofNoFill();
                ofSetLineWidth(2);
                ofDrawRectangle(0, topPosition, contentWidth, elementHeight);
                ofPopStyle();
                oscDrawIndex++;
            }
        }
        
        //Draw the Bars
        for(int i = 0; i < oscillatorBankIns.size(); i++){
            if(activeOscInCounter[i] > 0){
                int topPosition = (elementHeight * oscDrawIndex);
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
                oscDrawIndex++;
            }
        }
        
        
    }
    if(activeGridCounter > 0 ){
        ofRectangle gridRectangle(contentWidth, 0, ofGetWidth()-contentWidth, ofGetHeight()/3);
        int lin_size = gridIn.get().size();
        int width = 0, height = 0;
        if(pow(int(sqrt(lin_size)), 2) ==  lin_size){
            width = sqrt(lin_size);
            height = sqrt(lin_size);
        }
        else{
            for(int i = floor(sqrt(lin_size)); i > 0; i--){
                if((lin_size/i) == int(lin_size/i)){
                    height = i;
                    width = lin_size/i;
                    break;
                }
            }
        }
        ofTexture tex;
        tex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        unsigned char *data = new unsigned char[lin_size];
        for(int i = 0 ; i < lin_size ; i++){
            data[i] = gridIn.get()[i] * 255;
        }
        tex.loadData(data, width, height, GL_LUMINANCE);
        delete[] data;
        
        tex.draw(gridRectangle);
        ofPushStyle();
        ofSetColor(ofColor::indianRed);
        ofNoFill();
        ofSetLineWidth(2);
        ofDrawRectangle(gridRectangle);
        ofPopStyle();
    }
    
    
    //Draw notifiers
    //TODO: Here will be better not to erase the messages and use mouseScrollData to get the older messages.
    
    int lineHeigh = 15;
    
    ofRectangle debugRectangle;
    if(activeGridCounter > 0 ){
        debugRectangle = ofRectangle(contentWidth, ofGetHeight()/3, ofGetWidth()-contentWidth, 2*ofGetHeight()/3);
    }else{
        debugRectangle = ofRectangle(contentWidth, 0, ofGetWidth()-contentWidth, ofGetHeight());
    }
    
    while(logBuffer->getSize()*lineHeigh > debugRectangle.getHeight()) logBuffer->eraseLastLine();
    
    for (int i = 0; i < logBuffer->getSize(); i=i+10){
        string line = logBuffer->getLine(i);
        ofDrawBitmapString(line, debugRectangle.x, debugRectangle.y + (lineHeigh*(i+1)));
    }
    
    
    //Draw the framerate
    ofSetColor(255, 0,0);
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, ofGetHeight()-10);
}


void waveScope::inputListener(ofAbstractParameter &abs){
    for(int i = 0; i < groupBankIn.size() ; i++){
        if(abs.getName() == groupBankIn[i].getName()){
            activeGroupInCounter[i] = 20;
        }
    }
    for(int i = 0; i < colorGroupIn.size() ; i++){
        if(abs.getName() == colorGroupIn[i].getName()){
            activeColorInCounter[i] = 20;
        }
    }
    for(int i = 0; i < oscillatorBankIns.size() ; i++){
        if(abs.getName() == oscillatorBankIns[i].getName()){
            activeOscInCounter[i] = 20;
        }
    }
    if(abs.getName() == gridIn.getName()){
        activeGridCounter = 20;
    }
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
        shared_ptr<ofAppBaseWindow> mainWindow = shared_ptr<ofAppBaseWindow>(ofGetWindowPtr());
        
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
        prevSettings.setGLVersion(4,1);
        prevSettings.shareContextWith = mainWindow;
        prevWindow = ofCreateWindow(prevSettings);
        ofAddListener(prevWindow->events().draw, this, &waveScope::drawEvent);
        ofAddListener(prevWindow->events().mouseMoved, this, &waveScope::mouseMoved);
        ofAddListener(prevWindow->events().mousePressed, this, &waveScope::mousePressed);
        ofAddListener(prevWindow->events().mouseReleased, this, &waveScope::mouseReleased);
        ofAddListener(prevWindow->events().mouseDragged, this, &waveScope::mouseDragged);
        ofAppGLFWWindow * ofWindow = (ofAppGLFWWindow*)prevWindow.get();
        GLFWwindow * glfwWindow = ofWindow->getGLFWWindow();
        //glfwSetWindowCloseCallback(glfwWindow, window_no_close_scope);
    }
    else{
        prevWindowRect.setPosition(prevWindow->getWindowPosition());
        prevWindowRect.setSize(prevWindow->getWidth(), prevWindow->getHeight());
        prevWindow->setWindowShouldClose();
        prevWindow = nullptr;
    }
}
