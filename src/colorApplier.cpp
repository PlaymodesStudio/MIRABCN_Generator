//
//  colorApplier.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 01/12/2016.
//
//

#include "colorApplier.h"
#include "sharedResources.h"

colorApplier::colorApplier(int _id){
    parameters = new ofParameterGroup;
    parameters->setName("colorApplier " + ofToString(_id));
    parameters->add(bypass.set("Bypass", false));
    parameters->add(colorPickerParam[0].set("Color 1", ofColor::white));
    parameters->add(colorRParam[0].set("Color 1 R", 0, 0, 255));
    parameters->add(colorGParam[0].set("Color 1 G", 0, 0, 255));
    parameters->add(colorBParam[0].set("Color 1 B", 0, 0, 255));
    parameters->add(colorHParam[0].set("Color 1 Hue", 0, 0, 360));

    parameters->add(colorPickerParam[1].set("Color 2", ofColor::white));
    parameters->add(colorRParam[1].set("Color 2 R", 255, 0, 255));
    parameters->add(colorGParam[1].set("Color 2 G", 255, 0, 255));
    parameters->add(colorBParam[1].set("Color 2 B", 255, 0, 255));
    parameters->add(colorHParam[1].set("Color 2 Hue", 0, 0, 360));
    parameters->add(colorDisplacement.set("Color Displacement", 0, 0, 1));
    
//    parameters->add(randomColorStepsParam.set("Rnd Color Steps", 4, 0, 255));
//    sharedResources::addDropdownToParameterGroupFromParameters(parameters, "Rnd ChangeTypes", {"no", "on presset", "onTrigger"}, randomizeTypeColorParam);
    
    parameters->add(indexIn.set("Indexs", {0}));
    parameters->add(grayScaleIn.set("Input", {{0}}));
    parameters->add(gradientPreview.set("Gradient Preview", {{}}));
    parameters->add(colorizedValues.set("Output", {{ofColor::white}}));
    
    parametersControl::getInstance().createGuiFromParams(parameters);
    
    colorPickerParam[0].addListener(this, &colorApplier::colorListener);
    colorRParam[0].addListener(this, &colorApplier::colorSliderListener);
    colorGParam[0].addListener(this, &colorApplier::colorSliderListener);
    colorBParam[0].addListener(this, &colorApplier::colorSliderListener);
    
    colorPickerParam[1].addListener(this, &colorApplier::colorListener);
    colorRParam[1].addListener(this, &colorApplier::colorSliderListener);
    colorGParam[1].addListener(this, &colorApplier::colorSliderListener);
    colorBParam[1].addListener(this, &colorApplier::colorSliderListener);
    
    colorHParam[0].addListener(this, &colorApplier::colorHueListener);
    colorHParam[1].addListener(this, &colorApplier::colorHueListener);
    
    colorDisplacement.addListener(this, &colorApplier::colorDisplacementChanged);
    grayScaleIn.addListener(this, &colorApplier::applyColor);
    
    colorIsChanging = false;
}

void colorApplier::applyColor(vector<vector<float>> &inputVec){
    int w = inputVec.size();
    int h = inputVec[0].size();
    if(bypass){
        if(tempColors.size() != w || tempColors[0].size() != h){
            tempColors.resize(w, vector<ofColor>(h));
            tempGradient.resize(w, vector<ofColor>(h));
        }
        for(int i = 0 ; i < w ; i++){
            for (int j = 0 ; j < h ; j++){
                tempColors[i][j] =  colorPickerParam[0].get() * inputVec[i][j];
                tempGradient[i][j] = colorPickerParam[0].get();
            }
        }
        parameters->get("Output").cast<vector<vector<ofColor>>>() = tempColors;
        parameters->get("Gradient Preview").cast<vector<vector<ofColor>>>() = tempGradient;
    }else{
        if(tempColors.size() != w){
            tempColors.resize(w, vector<ofColor>(h));
            tempGradient.resize(w, vector<ofColor>(h));
        }
        
        if(colorDisplacementVector.size() != indexIn.get().size()){
            colorDisplacementVector.resize(indexIn.get().size(), {ofRandom(-colorDisplacement, colorDisplacement), ofRandom(-colorDisplacement, colorDisplacement), ofRandom(-colorDisplacement, colorDisplacement)});
        }
        
        if(indexIn.get().size() == w){
            for(int i = 0 ; i < w ; i++){
                ofFloatColor newColor = (colorPickerParam[0].get()*indexIn.get()[i])+(colorPickerParam[1].get()*(1-indexIn.get()[i]));
                newColor.r += (colorDisplacementVector[i][0]);
                newColor.g += (colorDisplacementVector[i][1]);
                newColor.b += (colorDisplacementVector[i][2]);
                for (int j = 0 ; j < h ; j++){
                    tempColors[i][j] =  newColor * inputVec[i][j];
                    tempGradient[i][j] = newColor;
                }
            }
            parameters->get("Output").cast<vector<vector<ofColor>>>() = tempColors;
            parameters->get("Gradient Preview").cast<vector<vector<ofColor>>>() = tempGradient;
        }
        else if(indexIn.get().size() == h){
            for(int j = 0 ; j < h ; j++){
                ofFloatColor newColor = (colorPickerParam[0].get()*indexIn.get()[j])+(colorPickerParam[1].get()*(1-indexIn.get()[j]));
                newColor.r += (colorDisplacementVector[j][0]);
                newColor.g += (colorDisplacementVector[j][1]);
                newColor.b += (colorDisplacementVector[j][2]);
                for (int i = 0 ; i < w ; i++){
                    tempColors[i][j] =  newColor * inputVec[i][j];
                    tempGradient[i][j] = newColor;
                }
            }
            parameters->get("Output").cast<vector<vector<ofColor>>>() = tempColors;
            parameters->get("Gradient Preview").cast<vector<vector<ofColor>>>() = tempGradient;
        }
    }
}

void colorApplier::colorDisplacementChanged(float &f){
    for(auto &randDisplacement : colorDisplacementVector){
        randDisplacement = {ofRandom(-f, f), ofRandom(-f, f), ofRandom(-f, f)};
    }
}

void colorApplier::colorListener(ofColor &c){
    if(!colorIsChanging){
        colorIsChanging = true;
        for(int i = 0; i < 2; i++){
            colorRParam[i] = colorPickerParam[i].get().r;
            colorGParam[i] = colorPickerParam[i].get().g;
            colorBParam[i] = colorPickerParam[i].get().b;
            colorHParam[i] = colorPickerParam[i].get().getHueAngle();
        }
        colorIsChanging = false;
    }
}

void colorApplier::colorSliderListener(int &i){
    if(!colorIsChanging){
        colorIsChanging = true;
        for(int i = 0; i < 2; i++){
            colorPickerParam[i] = ofColor(colorRParam[i], colorGParam[i], colorBParam[i]);
            colorHParam[i] = colorPickerParam[i].get().getHueAngle();
        }
        colorIsChanging = false;
    }
}

void colorApplier::colorHueListener(int &i){
    if(!colorIsChanging){
        colorIsChanging = true;
        for(int i = 0; i < 2; i++){
            ofColor color = colorPickerParam[i];
            color.setHueAngle(colorHParam[i]);
            colorPickerParam[i] = color;
            colorRParam[i] = colorPickerParam[i].get().r;
            colorGParam[i] = colorPickerParam[i].get().g;
            colorBParam[i] = colorPickerParam[i].get().b;
        }
        colorIsChanging = false;
    }
}
