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
    parameters->add(input.set("Input", {{}}));
    
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

void dataRecorder::inputListener(vector<vector<ofColor> > &info){
    if(record){
        int w = info.size();
        int h = info[0].size();
        ofImage img;
        if(invert){
            unsigned char *data = new unsigned char[w * h*3];
            for(int i = 0 ; i < h ; i++){
                for ( int j = 0; j < w ; j++){
                    data[(i*3)+h*3*j] = info[j][i].r;
                    data[(i*3)+(h*3*j)+1] = info[j][i].g;
                    data[(i*3)+(h*3*j)+2] = info[j][i].b;
                }
            }
//            const unsigned char *constData = data;
            img.setFromPixels(data, h, w, OF_IMAGE_COLOR);
            delete [] data;
        }else{
            unsigned char *data = new unsigned char[w * h*3];
            for(int i = 0 ; i < w ; i++){
                for ( int j = 0; j < h ; j++){
                    data[(i*3)+w*3*j] = info[i][j].r;
                    data[(i*3)+(w*3*j)+1] = info[i][j].g;
                    data[(i*3)+(w*3*j)+2] = info[i][j].b;
                }
            }
            img.setFromPixels(data, w, h, OF_IMAGE_COLOR);
            delete [] data;
        }
        img.save("recordings/" + filename.get() + "/" + filename.get() + "_" + ofToString(frameCounter, 4, '0') + ".png");
        frameCounter++;
    }
    
}

void dataRecorder::recordListener(bool &b){
    if(b){
        frameCounter = 0;
    }else{
        parameters->getBool("Auto Rec Loop") = false;
    }
}
