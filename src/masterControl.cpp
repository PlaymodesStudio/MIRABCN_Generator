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
    
    
    parameters.add(drawCurve_param.set("Draw Curve", 0));
    parameters.add(applyCurve_param.set("Apply Curve", 0));
}

void masterControl::computeOutTex(ofFbo &outTex, vector<float> infoVec, int i){
    outTex.begin();
    for(int j = 0; j < infoVec.size(); j++){
        int valueInByte = infoVec[j]*255;
        if(applyCurve_param)
            valueInByte = outputCurve[valueInByte];
        
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
        if(applyCurve_param)
            valueInByte = outputCurve[valueInByte];
        
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