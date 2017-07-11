//
//  colorApplier.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 01/12/2016.
//
//

#include "colorApplier.h"
#include "sharedResources.h"

colorApplier::colorApplier(){
    parameters = new ofParameterGroup;
    parameters->setName("colorApplier");
    parameters->add(colorPickerParam[0].set("Color 1 Picker", ofColor::white));
    parameters->add(colorRParam[0].set("Color 1 R", 1, 0, 1));
    parameters->add(colorGParam[0].set("Color 1 G", 1, 0, 1));
    parameters->add(colorBParam[0].set("Color 1 B", 1, 0, 1));

    parameters->add(colorPickerParam[1].set("Color 2 Picker", ofColor::white));
    parameters->add(colorRParam[1].set("Color 2 R", 1, 0, 1));
    parameters->add(colorGParam[1].set("Color 2 G", 1, 0, 1));
    parameters->add(colorBParam[1].set("Color 2 B", 1, 0, 1));
    parameters->add(colorDisplacement.set("Color Displacement", 0, 0, 1));
    
//    parameters->add(randomColorStepsParam.set("Rnd Color Steps", 4, 0, 255));
//    sharedResources::addDropdownToParameterGroupFromParameters(parameters, "Rnd ChangeTypes", {"no", "on presset", "onTrigger"}, randomizeTypeColorParam);
    
    parameters->add(indexIn.set("Indexs", {0}));
    parameters->add(grayScaleIn.set("Input", {{0}}));
    parameters->add(colorizedValues.set("Output", {{ofColor::white}}));
    
    parametersControl::getInstance().createGuiFromParams(parameters);
    
    
    colorDisplacement.addListener(this, &colorApplier::colorDisplacementChanged);
    grayScaleIn.addListener(this, &colorApplier::applyColor);
}

void colorApplier::applyColor(vector<vector<float>> &inputVec){
    int w = inputVec.size();
    int h = inputVec[0].size();
    if(tempColors.size() != w){
        tempColors.resize(w, vector<ofColor>(h));
    }
    
    if(colorDisplacementVector.size() != indexIn.get().size()){
        colorDisplacementVector.resize(w, {ofRandom(-colorDisplacement, colorDisplacement), ofRandom(-colorDisplacement, colorDisplacement), ofRandom(-colorDisplacement, colorDisplacement)});
    }
    
    if(indexIn.get().size() == w){
        for(int i = 0 ; i < w ; i++){
            ofFloatColor newColor = (colorPickerParam[0].get()*indexIn.get()[i])+(colorPickerParam[1].get()*(1-indexIn.get()[i]));
            newColor.r += (colorDisplacementVector[i][0]);
            newColor.g += (colorDisplacementVector[i][1]);
            newColor.b += (colorDisplacementVector[i][2]);
            for (int j = 0 ; j < h ; j++){
                tempColors[i][j] =  newColor * inputVec[i][j];
            }
        }
        parameters->get("Output").cast<vector<vector<ofColor>>>() = tempColors;
    }
    else if(indexIn.get().size() == h){
        for(int j = 0 ; j < h ; j++){
            ofFloatColor newColor = (colorPickerParam[0].get()*indexIn.get()[j])+(colorPickerParam[1].get()*(1-indexIn.get()[j]));
            newColor.r += (colorDisplacementVector[j][0]);
            newColor.g += (colorDisplacementVector[j][1]);
            newColor.b += (colorDisplacementVector[j][2]);
            for (int i = 0 ; i < w ; i++){
                tempColors[i][j] =  newColor * inputVec[i][j];
            }
        }
        parameters->get("Output").cast<vector<vector<ofColor>>>() = tempColors;
    }

}

void colorApplier::colorDisplacementChanged(float &f){
    for(auto &randDisplacement : colorDisplacementVector){
        randDisplacement = {ofRandom(-f, f), ofRandom(-f, f), ofRandom(-f, f)};
    }
}
