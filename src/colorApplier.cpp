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
    resources = &sharedResources::getInstance();
    
    parameters = new ofParameterGroup;
    parameters->setName("colorApplier " + ofToString(_id));
    parameters->add(reloadShaderParam.set("Reload Shader", false));
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
    
    parameters->add(modulationInfo[0].set("Mod in X", vector<float>(1, 0), vector<float>(1, 0), vector<float>(1, 1)));
    parameters->add(modulationInfo[1].set("Mod in Y", vector<float>(1, 0), vector<float>(1, 0), vector<float>(1, 1)));
    
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
//    reloadShaderParam.addListener(this, &colorApplier::reloadShader);
    
    imageTextureFile.addListener(this, &colorApplier::imageFileChangedListener);
    isImageLoaded = false;
    
    colorIsChanging = false;
    
    oldColorDisplacement = colorDisplacement;
    
    modulationInfoBuffer.allocate();
    modulationInfoBuffer.bind(GL_TEXTURE_BUFFER);
    modulationInfoBuffer.setData(vector<float>(1, -1), GL_STREAM_DRAW);
    modulationInfoTexture.allocateAsBufferTexture(modulationInfoBuffer, GL_R32F);
    
    
    bool sillyBool = true;
    reloadShader(sillyBool);
    
    modulationInfo[0].addListener(this, &colorApplier::modulationInfoListener);
    modulationInfo[1].addListener(this, &colorApplier::modulationInfoListener);

}

void colorApplier::reloadShader(bool &b){
    outputShader.load("Shaders/color.vert", "Shaders/color.frag");
    outputShader.begin();
    outputShader.setUniformTexture("modulationInfo", modulationInfoTexture, resources->getNextAvailableShaderTextureLocation());
    outputShader.end();
    
    previewShader.load("Shaders/color.vert", "Shaders/color.frag");
    previewShader.begin();
    previewShader.setUniformTexture("modulationInfo", modulationInfoTexture, resources->getNextAvailableShaderTextureLocation());
    previewShader.end();
    
    infoTextureOutputShaderTextureLocation = resources->getNextAvailableShaderTextureLocation();
    imageTextureOutputShaderTextureLocation = resources->getNextAvailableShaderTextureLocation();
    infoTexturePreviewShaderTextureLocation = resources->getNextAvailableShaderTextureLocation();
    imageTexturePreviewShaderTextureLocation = resources->getNextAvailableShaderTextureLocation();
}

void colorApplier::applyColor(ofTexture* &inputTex){
    width = inputTex->getWidth();
    height = inputTex->getHeight();
    if(outputFbo.getWidth() != width || outputFbo.getHeight() != height || !outputFbo.isAllocated()){
        outputFbo.allocate(width, height, GL_RGB);
        previewFbo.allocate(width, height, GL_RGB);
        whiteFbo.allocate(width, height, GL_RGB);
        whiteFbo.begin();
        ofSetColor(255, 255, 255);
        ofDrawRectangle(0, 0, width, height);
        whiteFbo.end();
        modulationInfoBuffer.setData(vector<float>(width+height, -1), GL_STREAM_DRAW);
    }
    
    
    if(isImageLoaded){
        if(imageTexture.getWidth() == height && imageTexture.getHeight() == width){
            imageTexture.rotate90(1);
        }
        else if(imageTexture.getWidth() == width && imageTexture.getHeight() == height){
            
        }
        else{
            isImageLoaded = false;
        }
    }
    

    outputFbo.begin();
    outputShader.begin();
    outputShader.setUniform1i("width", width);
    outputShader.setUniform1i("useImage", isImageLoaded);
    outputShader.setUniform3f("color1", colorPickerParam[0]->r/255., colorPickerParam[0]->g/255., colorPickerParam[0]->b/255.);
    outputShader.setUniform3f("color2", colorPickerParam[1]->r/255., colorPickerParam[1]->g/255., colorPickerParam[1]->b/255.);
    outputShader.setUniformTexture("inputTexture", *inputTex, infoTextureOutputShaderTextureLocation);
    if(isImageLoaded)
        outputShader.setUniformTexture("inputImage", imageTexture.getTexture(), imageTextureOutputShaderTextureLocation);

    inputTex->draw(0, 0);
    outputShader.end();
    outputFbo.end();
    parameters->get("Output").cast<ofTexture*>() = &outputFbo.getTexture();
    
    
    previewFbo.begin();
    previewShader.begin();
    previewShader.setUniform1i("width", width);
    outputShader.setUniform1i("useImage", isImageLoaded);
    previewShader.setUniform3f("color1", colorPickerParam[0]->r/255., colorPickerParam[0]->g/255., colorPickerParam[0]->b/255.);
    previewShader.setUniform3f("color2", colorPickerParam[1]->r/255., colorPickerParam[1]->g/255., colorPickerParam[1]->b/255.);
    previewShader.setUniformTexture("inputTexture", whiteFbo.getTexture(), infoTexturePreviewShaderTextureLocation);
    if(isImageLoaded)
        outputShader.setUniformTexture("inputImage", imageTexture.getTexture(), imageTexturePreviewShaderTextureLocation);

    inputTex->draw(0, 0);
    previewShader.end();
    previewFbo.end();
    parameters->get("Gradient Preview").cast<ofTexture*>() = &previewFbo.getTexture();
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

void colorApplier::modulationInfoListener(vector<float> &vf){
    if(&vf == &modulationInfo[0].get()){
        if(vf.size() == 1){
            modulationInfoBuffer.updateData(0, vector<float>(width, -1));
        }else if(vf.size() == width){
            modulationInfoBuffer.updateData(0, vf);
        }
    }
    if(&vf == &modulationInfo[1].get()){
        if(vf.size() == 1){
            modulationInfoBuffer.updateData(width*4, vector<float>(height, -1));
        }else if(vf.size() == height){
            modulationInfoBuffer.updateData(width*4, vf);
        }
    }
}

