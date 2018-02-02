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
    parameters->add(colorRParam[0].set("Color 1 R", 255, 0, 255));
    parameters->add(colorGParam[0].set("Color 1 G", 255, 0, 255));
    parameters->add(colorBParam[0].set("Color 1 B", 255, 0, 255));
    parameters->add(colorHParam[0].set("Color 1 Hue", 0, 0, 360));

    parameters->add(colorPickerParam[1].set("Color 2", ofColor::white));
    parameters->add(colorRParam[1].set("Color 2 R", 0, 0, 255));
    parameters->add(colorGParam[1].set("Color 2 G", 0, 0, 255));
    parameters->add(colorBParam[1].set("Color 2 B", 0, 0, 255));
    parameters->add(colorHParam[1].set("Color 2 Hue", 0, 0, 360));
    parameters->add(colorDisplacement.set("Color Displacement", 0, 0, 1));
    
//    parameters->add(randomColorStepsParam.set("Rnd Color Steps", 4, 0, 255));
//    sharedResources::addDropdownToParameterGroupFromParameters(parameters, "Rnd ChangeTypes", {"no", "on presset", "onTrigger"}, randomizeTypeColorParam);
    
    parameters->add(imageTextureFile.set("Img Tex File", ""));
    
    parameters->add(indexInX.set("Indexs", {0}, {0}, {1}));
    parameters->add(indexInY.set("Indexs Y", {0}, {0}, {1}));
    parameters->add(grayScaleIn.set("Input", nullptr));
    parameters->add(gradientPreview.set("Gradient Preview", nullptr));
    parameters->add(colorizedValues.set("Output", nullptr));
    
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
    
    //colorDisplacement.addListener(this, &colorApplier::colorDisplacementChanged);
    grayScaleIn.addListener(this, &colorApplier::applyColor);
    
    imageTextureFile.addListener(this, &colorApplier::imageFileChangedListener);
    isImageLoaded = false;
    
    colorIsChanging = false;
    
    oldColorDisplacement = colorDisplacement;
}

void colorApplier::applyColor(ofTexture* &inputTex){
    int w = inputTex->getWidth();
    int h = inputTex->getHeight();
    if(outputFbo.getWidth() != w || outputFbo.getHeight() != h || !outputFbo.isAllocated()){
        outputFbo.allocate(w, h, GL_RGB);
        gradientFbo.allocate(w, h, GL_RGB);
    }
    if(bypass){
//        if(isImageLoaded){
//            if(imageTexture.getWidth() == h && imageTexture.getHeight() == w){
//                imageTexture.rotate90(1);
//            }
//            if(imageTexture.getWidth() == w && imageTexture.getHeight() == h){
////                for(int i = 0 ; i < w ; i++){
////                    for (int j = 0 ; j < h ; j++){
////                        tempColors[i][j] =  imageTexture.getColor(i, j) * inputVec[i][j];
////                        tempGradient[i][j] = imageTexture.getColor(i, j);
////                    }
////                }
////                ofPixels imagePix = imageTexture.getPixels();
////                ofPixels inputPix;
////                inputPix.allocate(imageTexture.getWidth(), imageTexture.getHeight(), 3);
////                inputTex->readToPixels(inputPix);
////                for(int i = 0; i < imgePix.size(); i++){
////                    tempColors = imageTexture.getTexture() * *inputTex;
////                }
//                //gradientFbo = imageTexture.getTexture();
//            }else{
//                isImageLoaded = false;
//            }
//        }
//        else
        {
            outputFbo.begin();
            ofClear(255,255,255,0);
            ofSetColor(colorPickerParam[0].get());
            inputTex->draw(0,0);
            outputFbo.end();
            
            gradientFbo.begin();
            ofClear(255,255,255,0);
            ofSetColor(colorPickerParam[0].get());
            ofDrawRectangle(0, 0, w, h);
            gradientFbo.end();
        }
        parameters->get("Output").cast<ofTexture*>() = &outputFbo.getTexture();
        parameters->get("Gradient Preview").cast<ofTexture*>() = &gradientFbo.getTexture();
    }
    else{
//        if(colorDisplacementTexture.getWidth() != w || colorDisplacementTexture.getHeight() != h || !colorDisplacementTexture.isAllocated()){
//            colorDisplacementTexture.allocate(w, h, GL_R);
//            computeNewColorDisplacement(colorDisplacement);
//        }
//
//        ofPixels colorDisplacementPixels;
//        colorDisplacementPixels.allocate(w, h, 1);
//        colorDisplacementTexture.readToPixels(colorDisplacementPixels);
        
        
        bool validXIndex = false;
        bool validYIndex = false;
        if(indexInX.get().size() == w) validXIndex = true;
        if(indexInY.get().size() == h) validYIndex = true;
            
        ofPixels inputPixels;
        inputPixels.allocate(w, h, 3);
        inputTex->readToPixels(inputPixels);
        outputFbo.begin();
        for(int i = 0; i < w; i++){
            for(int j = 0; j < h; j++){
                float indexMix = 0;
                if(validXIndex && validYIndex){
                    indexMix = (indexInX.get()[i] + indexInY.get()[j]) / 2;
                }else if(validYIndex){
                    indexMix = indexInY.get()[j];
                }else if(validXIndex){
                    indexMix = indexInX.get()[i];
                }
                ofFloatColor newColor = (colorPickerParam[0].get()*indexMix)+(colorPickerParam[1].get()*(1-indexMix));
//                newColor.r += (colorDisplacementPixels[(i*w)+j]);
//                newColor.g += (colorDisplacementPixels[(i*w)+j+1]);
//                newColor.b += (colorDisplacementPixels[(i*w)+j+2]);
                
                newColor.r *= (float)inputPixels[(j*h*3)+(i*3)]/255.0;
                newColor.g *= (float)inputPixels[(j*h*3)+(i*3)+1]/255.0;
                newColor.b *= (float)inputPixels[(j*h*3)+(i*3)+2]/255.0;
                
                ofSetColor(newColor);
                ofDrawRectangle(i, j, 1, 1);
                
                //                    pix[(i*w*3)+(j*3)] = newColor.r + (colorDisplacementPixels[(i*w*3)+(j*3)]);
                //                    pix[(i*w*3)+(j*3)+1] = newColor.g + (colorDisplacementPixels[(i*w*3)+(j*3)+1]);
                //                    pix[(i*w*3)+(j*3)+2] = newColor.b + (colorDisplacementPixels[(i*w*3)+(j*3)+2]);
            }
        }
        outputFbo.end();
        
        
        
        parameters->get("Output").cast<ofTexture*>() = &outputFbo.getTexture();
        //            parameters->get("Gradient Preview").cast<vector<vector<ofColor>>>() = tempGradient;
        
        
//        if(indexIn.get().size() == w){
//            for(int i = 0 ; i < w ; i++){
//                ofFloatColor newColor = (colorPickerParam[0].get()*indexIn.get()[i])+(colorPickerParam[1].get()*(1-indexIn.get()[i]));
//                newColor.r += (colorDisplacementVector[i][0]);
//                newColor.g += (colorDisplacementVector[i][1]);
//                newColor.b += (colorDisplacementVector[i][2]);
//                for (int j = 0 ; j < h ; j++){
//                    tempColors[i][j] =  newColor * inputVec[i][j];
//                    tempGradient[i][j] = newColor;
//                }
//            }
//            parameters->get("Output").cast<vector<vector<ofColor>>>() = tempColors;
//            parameters->get("Gradient Preview").cast<vector<vector<ofColor>>>() = tempGradient;
//        }
//        else if(indexIn.get().size() == h){
//            for(int j = 0 ; j < h ; j++){
//                ofFloatColor newColor = (colorPickerParam[0].get()*indexIn.get()[j])+(colorPickerParam[1].get()*(1-indexIn.get()[j]));
//                newColor.r += (colorDisplacementVector[j][0]);
//                newColor.g += (colorDisplacementVector[j][1]);
//                newColor.b += (colorDisplacementVector[j][2]);
//                for (int i = 0 ; i < w ; i++){
//                    tempColors[i][j] =  newColor * inputVec[i][j];
//                    tempGradient[i][j] = newColor;
//                }
//            }
//            parameters->get("Output").cast<vector<vector<ofColor>>>() = tempColors;
//            parameters->get("Gradient Preview").cast<vector<vector<ofColor>>>() = tempGradient;
//        }
    }
}

void colorApplier::colorDisplacementChanged(float &f){
//    if(oldColorDisplacement != f){
//        for(auto &randDisplacement : colorDisplacementVector){
//            randDisplacement = {ofRandom(-f, f), ofRandom(-f, f), ofRandom(-f, f)};
//        }
//    }
//    oldColorDisplacement = f;
}

void colorApplier::computeNewColorDisplacement(float f){
    ofPixels pix;
    pix.allocate(colorDisplacementTexture.getWidth(), colorDisplacementTexture.getHeight(), 1);
    for(int i = 0; i < pix.size(); i++){
        pix[i] = ofRandom(-f, f);
    }
    colorDisplacementTexture.loadData(pix);
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

void colorApplier::imageFileChangedListener(string &s){
    if(s != "")
        isImageLoaded = imageTexture.load("colorTextures/" + s);
}
