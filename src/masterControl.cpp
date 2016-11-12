//
//  masterControl.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 27/10/16.
//
//

#include "masterControl.h"

void masterControl::setup(int index){
    parameters.setName("master " + ofToString(index));
    
    parameters.add(masterFader.set("Master Fader", 1, 0, 1));
    parameters.add(previewTex.set("Preview Texutre", 0));
    
    ofParameterGroup invertDropDown;
    invertDropDown.setName("Inversion Type");
    ofParameter<string> tempInvStrParam("Options", "no-|-position inverse-|-value inverse");
    invertDropDown.add(tempInvStrParam);
    invertDropDown.add(inversionType.set("Inversion Type", 0, 0, 3));
    parameters.add(invertDropDown);
    
    parameters.add(colorPickerParam.set("LedsColor", ofColor::white));
    parameters.add(colorRParam.set("R Channel", colorPickerParam.get().r, 0, 255));
    parameters.add(colorGParam.set("G Channel", colorPickerParam.get().g, 0, 255));
    parameters.add(colorBParam.set("B Channel", colorPickerParam.get().b, 0, 255));
    parameters.add(randomColorStepsParam.set("Rnd Color Steps", 4, 0, 255));
    parameters.add(randomizeColorParam.set("Randomize Color", true));
    
    colorPickerParam.addListener(this, &masterControl::onColorPickerChanged);
    colorRParam.addListener(this, &masterControl::onColorSlidersChanged);
    colorGParam.addListener(this, &masterControl::onColorSlidersChanged);
    colorBParam.addListener(this, &masterControl::onColorSlidersChanged);
    
    parameters.add(drawCurve_param.set("Draw Curve", 0));
    parameters.add(applyCurve_param.set("Apply Curve", 0));
    
}

void masterControl::computeOutTex(ofFbo &outTex, ofFbo &outTintedTex, vector<float> infoVec, int i){
    outTintedTex.begin();
    for(int j = 0; j < infoVec.size(); j++){
        int valueInByte = infoVec[j]*255;
        if(applyCurve_param)
            valueInByte = outputCurve[valueInByte];
        
        ofSetColor(colorPickerParam.get() * (valueInByte*masterFader/255));
        if(previewTex){
            ofDrawRectangle(i,j, 1, 1);
        }else{
            ofDrawRectangle(2*i,j, 1, 1);
        }
    }
    for(int j = 0; j < infoVec.size(); j++){
        int valueInByte;
        switch (inversionType) {
            case 0:
                valueInByte = infoVec[j]*255;
                break;
                
            case 1:
                valueInByte = infoVec[infoVec.size()-1-j]*255;
                break;
                
            case 2:
                valueInByte = (1-infoVec[j])*255;
                break;
                
            default:
                valueInByte = infoVec[j]*255;
                break;
        }
        if(applyCurve_param)
            valueInByte = outputCurve[valueInByte];
        
        ofSetColor(colorPickerParam.get() * (valueInByte*masterFader/255));
        
        if(previewTex){
            ofDrawRectangle(i+12,j, 1, 1);
        }else{
            ofDrawRectangle(2*i+1,j, 1, 1);
        }
        
    }
    outTintedTex.end();
    
    outTex.begin();
    for(int j = 0; j < infoVec.size(); j++){
        int valueInByte = infoVec[j]*255;
        
        ofSetColor(valueInByte*masterFader);
        if(previewTex){
            ofDrawRectangle(i,j, 1, 1);
        }else{
            ofDrawRectangle(2*i,j, 1, 1);
        }
    }
    for(int j = 0; j < infoVec.size(); j++){
        int valueInByte;
        switch (inversionType) {
            case 0:
                valueInByte = infoVec[j]*255;
                break;
                
            case 1:
                valueInByte = infoVec[infoVec.size()-1-j]*255;
                break;
                
            case 2:
                valueInByte = (1-infoVec[j])*255;
                break;
                
            default:
                valueInByte = infoVec[j]*255;
                break;
        }
        ofSetColor(valueInByte*masterFader);
        
        if(previewTex){
            ofDrawRectangle(i+12,j, 1, 1);
        }else{
            ofDrawRectangle(2*i+1,j, 1, 1);
        }
        
    }
    outTex.end();
}

void masterControl::computeWaveTex(ofFbo &tex, float value, ofPoint pos){

    tex.begin();
    ofSetColor(value*255);
    ofDrawRectangle(pos.x, pos.y, 1, 1);
    tex.end();
}

void masterControl::computeLinWaveTex(ofFbo &tex, float value, int index){
    //Draw info to the FBO's
    tex.begin();
    
    //Draw the Bars
    float hei = tex.getHeight();
//    ofSetColor(0);
//    ofDrawRectangle(index, hei, 1, hei);
    
    ofSetColor(255);
    ofDrawRectangle(index, (1-value)*hei, 1, value*hei);
    tex.end();
    
}

void masterControl::tintTexture(ofFbo &tex){
    ofTexture& texture = tex.getTexture();
    tex.begin();
    ofPushStyle();
    ofSetColor(colorPickerParam);
    texture.draw(0, 0);
    ofPopStyle();
    tex.end();
}

void masterControl::onColorPickerChanged(ofColor &color){
    colorRParam.setWithoutEventNotifications(color.r);
    colorGParam.setWithoutEventNotifications(color.g);
    colorBParam.setWithoutEventNotifications(color.b);
}

void masterControl::onColorSlidersChanged(int &component){
    colorPickerParam.set(ofColor(colorRParam, colorGParam, colorBParam));
}