//
//  oscillatorTexture.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 24/01/2018.
//
//

#include "oscillatorTexture.h"
#include "parametersControl.h"

oscillatorTexture::oscillatorTexture(int bankId, int xSize, int ySize, ofPoint pos){
    parameters = new ofParameterGroup();
    
    width = xSize;
    height = ySize;
    
    fbo.allocate(width, height, GL_RGB);
    
    
    
    parameters->setName("oscillatorTexture " + ofToString(bankId));
    parameters->add(reloadShaderParam.set("Reload Shader", false));
    reloadShaderParam.addListener(this, &oscillatorTexture::reloadShader);
    
    indexers.push_back(new baseIndexer(xSize));
    indexers.push_back(new baseIndexer(ySize));
    
    
    for(int i = 0; i < indexers.size(); i++){
        string dimensionStr = " ";
        int dimensionSize = 0;
        if(i == 0){
            dimensionStr = "X";
            dimensionSize = xSize;
        }else if(i == 1){
            dimensionStr = "Y";
            dimensionSize = ySize;
        }
        
        ofParameter<string> strlbl;
        parameters->add(strlbl.set(dimensionStr + " Dim_label", " "));
        
        parameters->add(indexers[i]->numWaves_Param.set("Num Waves " + dimensionStr, 1, 0, dimensionSize));
        parameters->add(indexers[i]->indexInvert_Param.set("Index Invert " + dimensionStr, 0, 0, 1));
        parameters->add(indexers[i]->symmetry_Param.set("Symmetry " + dimensionStr, 0, 0, 10));
        parameters->add(indexers[i]->indexRand_Param.set("Index Random " + dimensionStr, 0, 0, 1));
        parameters->add(indexers[i]->indexOffset_Param.set("Index Offset " + dimensionStr, 0, -dimensionSize/2, dimensionSize/2));
        parameters->add(indexers[i]->indexQuant_Param.set("Index Quantization " + dimensionStr, dimensionSize, 1, dimensionSize));
        parameters->add(indexers[i]->combination_Param.set("Index Combination " + dimensionStr, 0, 0, 1));
        parameters->add(indexers[i]->modulo_Param.set("Index Modulo " + dimensionStr, dimensionSize, 1, dimensionSize));
    }
    
    parameters->add(phasorIn.set("Phasor In", 0, 0, 1));
    
    parameters->add(xPhaseOffset.set("Phase Offset X", {0}, {0}, {1}));
    parameters->add(yPhaseOffset.set("Phase Offset Y", {0}, {0}, {1}));
    
    parameters->add(randomAdd_Param.set("Random Addition", 0, -.5, .5));
    parameters->add(scale_Param.set("Scale", 1, 0, 2));
    parameters->add(offset_Param.set("Offset", 0, -1, 1));
    parameters->add(pow_Param.set("Pow", 0, -40, 40));
    parameters->add(biPow_Param.set("Bi Pow", 0, -40, 40));
    parameters->add(quant_Param.set("Quantization", 255, 1, 255));
    parameters->add(pulseWidth_Param.set("Pulse Width", 1, 0, 1));
    parameters->add(skew_Param.set("Skew", 0, -1, 1));
    parameters->add(amplitude_Param.set("Fader", 1, 0, 1));
    parameters->add(invert_Param.set("Invert", 0, 0, 1));
    ofParameterGroup waveDropDown;
    waveDropDown.setName("Wave Select");
    ofParameter<string> tempStrParam("Options", "sin-|-cos-|-tri-|-square-|-saw-|-inverted saw-|-rand1-|-rand2");
    waveDropDown.add(tempStrParam);
    waveDropDown.add(waveSelect_Param.set("Wave Select", 0, 0, 7));
    parameters->add(waveDropDown);
    
    parameters->add(oscillatorOut.set("Oscillator Out", nullptr));
    
    
    
    
    //TBOs
    //xIndex
    xIndexBuffer.allocate();
    xIndexBuffer.bind(GL_TEXTURE_BUFFER);
    xIndexBuffer.setData(indexers[0]->getIndexs(), GL_STREAM_DRAW);
    xIndexTexture.allocateAsBufferTexture(xIndexBuffer, GL_R32F);
    
    
    //yIndex
    yIndexBuffer.allocate();
    yIndexBuffer.bind(GL_TEXTURE_BUFFER);
    yIndexBuffer.setData(indexers[1]->getIndexs(), GL_STREAM_DRAW);
    yIndexTexture.allocateAsBufferTexture(yIndexBuffer, GL_R32F);
    
    //Phase offset
    xPhaseOffsetBuffer.allocate();
    xPhaseOffsetBuffer.bind(GL_TEXTURE_BUFFER);
    xPhaseOffsetBuffer.setData(vector<float>(width, xPhaseOffset.get()[0]), GL_STREAM_DRAW);
    xPhaseOffsetTexture.allocateAsBufferTexture(xPhaseOffsetBuffer, GL_R32F);
    
    yPhaseOffsetBuffer.allocate();
    yPhaseOffsetBuffer.bind(GL_TEXTURE_BUFFER);
    yPhaseOffsetBuffer.setData(vector<float>(height, yPhaseOffset.get()[0]), GL_STREAM_DRAW);
    yPhaseOffsetTexture.allocateAsBufferTexture(yPhaseOffsetBuffer, GL_R32F);
    
    
    //LoadShader
    bool b = true;
    reloadShader(b);

    
    
    
    
    
    phasorIn.addListener(this, &oscillatorTexture::newPhasorIn);
    
    xPhaseOffset.addListener(this, &oscillatorTexture::xPhaseOffsetListener);
    yPhaseOffset.addListener(this, &oscillatorTexture::yPhaseOffsetListener);

    randomAdd_Param.addListener(this, &oscillatorTexture::newRandomAddParam);
    scale_Param.addListener(this, &oscillatorTexture::newScaleParam);
    offset_Param.addListener(this, &oscillatorTexture::newOffsetParam);
    pow_Param.addListener(this, &oscillatorTexture::newPowParam);
    quant_Param.addListener(this, &oscillatorTexture::newQuantParam);
    amplitude_Param.addListener(this, &oscillatorTexture::newAmplitudeParam);
    invert_Param.addListener(this, &oscillatorTexture::newInvertParam);
    biPow_Param.addListener(this, &oscillatorTexture::newBiPowParam);
    waveSelect_Param.addListener(this, &oscillatorTexture::newWaveSelectParam);
    pulseWidth_Param.addListener(this, &oscillatorTexture::newpulseWidthParam);
    skew_Param.addListener(this, &oscillatorTexture::newSkewParam);
    
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::red, pos);
    
    ofAddListener(parameters->parameterChangedE(), this,  &oscillatorTexture::parameterChanged);
}

void oscillatorTexture::parameterChanged(ofAbstractParameter &p){
    
}

void oscillatorTexture::reloadShader(bool &b){
    shader.load("noise.vert", "noise.frag");
    shader.begin();
    shader.setUniformTexture("xIndexs", xIndexTexture, 0);
    shader.setUniformTexture("yIndexs", yIndexTexture, 1);
    shader.setUniformTexture("xPhaseOffset", xPhaseOffsetTexture, 2);
    shader.setUniformTexture("yPhaseOffset", yPhaseOffsetTexture, 3);
    shader.end();
}

ofTexture& oscillatorTexture::computeBank(float phasor){
//    int i_x = 0, i_y = 0;
//    for(int i = 0; i < oscillators.size(); i++){
//        i_y = floor(i/width);
//        i_x = i - (i_y * width);
//        result[i_x][i_y] = oscillators[i]->computeFunc(phasor);
//    }
//
    fbo.begin();
    shader.begin();
    shader.setUniform1f("phase", phasor);
    shader.setUniform1f("time", ofGetElapsedTimef());
    ofDrawRectangle(0, 0, width, height);
    shader.end();
    fbo.end();
    
    ofPixels pixels;
    //pixels.allocate(width, height, GL_RGB);
    //fbo.getTexture().readToPixels(pixels);
//    for(int i = 0; i < width; i++){
//        for (int j = 0; j < height; j++){
//            result[i][j] = (float)pixels.getLine(j).getPixel(i).getColor().r / 255.0;
//        }
//    }
    
    return fbo.getTexture();
}

void oscillatorTexture::computeBidirectionalIndexs(){
    int lin_size = width * height;
    vector<float> tempOut(lin_size);
    if(width != 0 && height != 0){
        int width_i = 0, height_i = 0;
        for(int i = 0; i < lin_size; i++){
            height_i = floor(i/width);
            width_i = i - (height_i * width);
            oscillators[i]->setIndexNormalized(indexers[0]->getIndexs()[width_i]+ indexers[1]->getIndexs()[height_i]);
        }
    }
}

void oscillatorTexture::newPhasorIn(float &f){
    if(indexers[0]->areNewIndexs()){
        xIndexBuffer.updateData(0, indexers[0]->getIndexs());
    }
    if(indexers[1]->areNewIndexs()){
        yIndexBuffer.updateData(0, indexers[1]->getIndexs());
    }
    parameters->get("Oscillator Out").cast<ofTexture*>() = &computeBank(f);
}

#pragma mark PHASE OFFSET

void oscillatorTexture::xPhaseOffsetListener(vector<float> &vf){
    if(vf.size() == 1){
        xPhaseOffsetBuffer.updateData(0, vector<float>(width, vf[0]));
    }else if(vf.size() == width){
        xPhaseOffsetBuffer.updateData(0, vf);
    }
}

void oscillatorTexture::yPhaseOffsetListener(vector<float> &vf){
    if(vf.size() == 1){
        yPhaseOffsetBuffer.updateData(0, vector<float>(height, vf[0]));
    }else if(vf.size() == height){
        yPhaseOffsetBuffer.updateData(0, vf);
    }
}

void oscillatorTexture::newPowParam(float &f){
   
}

void oscillatorTexture::newpulseWidthParam(float &f){

}

void oscillatorTexture::newHoldTimeParam(float &f){

}

void oscillatorTexture::newQuantParam(int &i){

}

void oscillatorTexture::newScaleParam(float &f){

}

void oscillatorTexture::newOffsetParam(float &f){

}

void oscillatorTexture::newRandomAddParam(float &f){

}

void oscillatorTexture::newWaveSelectParam(int &i){

}

void oscillatorTexture::newAmplitudeParam(float &f){

}

void oscillatorTexture::newInvertParam(float &f){

}

void oscillatorTexture::newSkewParam(float &f){

}

void oscillatorTexture::newBiPowParam(float &f){

}
