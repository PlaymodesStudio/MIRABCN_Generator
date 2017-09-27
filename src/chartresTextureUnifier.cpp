//
//  chartresTextureUnifier.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 29/08/2017.
//
//

#include "chartresTextureUnifier.h"
#include "parametersControl.h"

chartresTextureUnifier::chartresTextureUnifier(){
    parameters = new ofParameterGroup();
    parameters->setName("chartresTextureUnifier");
    parameters->add(in288_8.set("In 288x8", {{}}));
    parameters->add(in1008_6.set("In 1008x6", {{}}));
    parameters->add(in8_2.set("In 8x2", {{}}));
    parameters->add(output.set("Output", {{}}));
    
    in288_8.addListener(this, &chartresTextureUnifier::computeOutput);
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::darkMagenta);
}

void chartresTextureUnifier::computeOutput(vector<vector<ofColor> > &in){
    vector<vector<ofColor>> outInfo;
    int width = 1008;
    int height = 20;
    outInfo.resize(20, vector<ofColor>(1008, ofColor::black));
    
    int currentLine = 1;
//    for(int i = 0; i < width; i++){
//        outInfo[currentLine][i] = ofColor::black;
//    }
//    currentLine++;
    
    if(in1008_6.get().size() == 6){
        for(int j = 0; j < 6; j++){
            for(int i = 0; i < width; i++){
                outInfo[currentLine][i] = in1008_6.get()[j][i];
            }
            currentLine++;
        }
    }else{
        currentLine += 6;
    }
    
    currentLine++;
    if(in288_8.get().size() == 8){
        for(int j = 0; j < 8; j++){
            for(int i = 0; i < 288; i++){
                outInfo[currentLine][i] = in288_8.get()[j][i];
            }
            currentLine++;
        }
    }
    else{
        currentLine += 8;
    }
    
    currentLine++;
    if(in8_2.get().size() == 2){
        for(int j = 0; j < 2; j++){
            for(int i = 0; i < 8; i++){
                outInfo[currentLine][i] = in8_2.get()[j][i];
            }
            currentLine++;
        }
    }else{
        currentLine += 2;
    }
    
    parameters->get("Output").cast<vector<vector<ofColor>>>() = outInfo;
}
