//
//  colorApplier.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 01/12/2016.
//
//

#include "colorApplier.h"
#include "sharedResources.h"

colorApplier::colorApplier(int nOscillators) : baseIndexer(nOscillators) {
    //parameters = new ofParameterGroup;
    parameters->setName("colorApplier 1");
    parameters->add(colorPickerParam[0].set("Color 1 Picker", ofColor::white));
    parameters->add(colorRParam[0].set("Color 1 R", 1, 0, 1));
    parameters->add(colorGParam[0].set("Color 1 G", 1, 0, 1));
    parameters->add(colorBParam[0].set("Color 1 B", 1, 0, 1));

    parameters->add(colorPickerParam[1].set("Color 2 Picker", ofColor::white));
    parameters->add(colorRParam[1].set("Color 2 R", 1, 0, 1));
    parameters->add(colorGParam[1].set("Color 2 G", 1, 0, 1));
    parameters->add(colorBParam[1].set("Color 2 B", 1, 0, 1));
    
    parameters->add(randomColorStepsParam.set("Rnd Color Steps", 4, 0, 255));
    sharedResources::addDropdownToParameterGroupFromParameters(parameters, "Rnd ChangeTypes", {"no", "on presset", "onTrigger"}, randomizeTypeColorParam);
    
//    parameters->add(indexs.set("Indexs", {0}));
    parameters->add(grayScaleIn.set("Input", {{0}}));
    parameters->add(colorizedValues.set("Output", {{ofColor::white}}));
    
    parametersControl::getInstance().createGuiFromParams(parameters);
    
    grayScaleIn.addListener(this, &colorApplier::applyColor);
}

void colorApplier::applyColor(vector<vector<float>> &inputVec){
    int w = inputVec.size();
    int h = inputVec[0].size();
    vector<vector<ofColor>> tempColors;
    tempColors.resize(w, vector<ofColor>(h));
    
    for(int i = 0 ; i < w ; i++){
        ofColor newColor = (colorPickerParam[0].get()*indexs[i])+(colorPickerParam[1].get()*(1-indexs[i]));
        for (int j = 0 ; j < h ; j++){
            tempColors[i][j] =  newColor * inputVec[i][j];
        }
    }
    parameters->get("Output").cast<vector<vector<ofColor>>>() = tempColors;
}
