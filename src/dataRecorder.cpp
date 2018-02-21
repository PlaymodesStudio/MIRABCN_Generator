//
//  dataRecorder.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 31/08/2017.
//
//

#include "dataRecorder.h"
#include "parametersControl.h"

dataRecorder::dataRecorder(){
    parameters = new ofParameterGroup;
    parameters->setName("dataRecorder");
    
    parameters->add(phasorIn.set("Phasor In", 0, 0, 1));
    parameters->add(record.set("Record", false));
    parameters->add(autoRecLoop.set("Auto Rec Loop", false));
    parameters->add(invert.set("Invert", true));
    parameters->add(filename.set("Filename", "recTest"));
    parameters->add(input.set("Input", nullptr));
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::green);
    
    phasorIn.addListener(this, &dataRecorder::phasorInListener);
    record.addListener(this, &dataRecorder::recordListener);
    input.addListener(this, &dataRecorder::inputListener);
    oldPhasor = 0;
    frameCounter = 0;
}

void dataRecorder::phasorInListener(float &f){
    if(autoRecLoop){
        if(f < oldPhasor){
            parameters->getBool("Record") = !record;
        }
    }
    oldPhasor = f;
}

void dataRecorder::inputListener(ofTexture* &info){
    if(record){
        ofImage img;
        if(invert){

        }else{
            info->readToPixels(img.getPixels());
        }
        img.save("recordings/" + filename.get() +  "_" + initRecordingTimestamp + "/" + filename.get() + "_" + ofToString(frameCounter, 4, '0') + ".png");
        frameCounter++;
    }
}

void dataRecorder::recordListener(bool &b){
    if(b){
        initRecordingTimestamp = ofGetTimestampString();
        frameCounter = 0;
    }else{
        parameters->getBool("Auto Rec Loop") = false;
    }
}
