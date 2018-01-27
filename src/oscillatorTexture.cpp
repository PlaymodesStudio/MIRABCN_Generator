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
    
//    parameters->add(phaseOffset[0].set("Phase Offset X", {0}, {0}, {1}));
//    parameters->add(phaseOffset[1].set("Phase Offset Y", {0}, {0}, {1}));
    
    auto setAndBindXYParamsVecFloat = [this](ofParameter<vector<float>> *p, string name, float val, float min, float max) -> void{
        parameters->add(p[0].set(name + " X", vector<float>(1, val), vector<float>(1, min), vector<float>(1, max)));
        parameters->add(p[1].set(name + " Y", vector<float>(1, val), vector<float>(1, min), vector<float>(1, max)));
    };
    
    auto setAndBindXYParamsVecInt = [this](ofParameter<vector<int>> *p, string name, int val, int min, int max) -> void{
        parameters->add(p[0].set(name + " X", vector<int>(1, val), vector<int>(1, min), vector<int>(1, max)));
        parameters->add(p[1].set(name + " Y", vector<int>(1, val), vector<int>(1, min), vector<int>(1, max)));
    };
    
    setAndBindXYParamsVecFloat(phaseOffset, "Phase Offset", 0, 0, 1);
    setAndBindXYParamsVecFloat(randomAddition, "Rnd Add", 0, -.5, .5);
    setAndBindXYParamsVecFloat(scale, "Scale", 1, 0, 2);
    setAndBindXYParamsVecFloat(offset, "Offset", 0, -1, 1);
    setAndBindXYParamsVecFloat(pow, "Pow", 0, -40, 40);
    setAndBindXYParamsVecFloat(bipow, "Bi Pow", 0, -40, 40);
    setAndBindXYParamsVecInt(quantization, "Quantize", 255, 1, 255);
    setAndBindXYParamsVecFloat(pulseWidth, "Pulse Width", 1, 0, 1);
    setAndBindXYParamsVecFloat(skew, "Skew", 0, -1, 1);
    setAndBindXYParamsVecFloat(fader, "Fader", 1, 0, 1);
    setAndBindXYParamsVecFloat(invert, "Invert", 0, 0, 1);
    setAndBindXYParamsVecFloat(waveform, "WaveForm", 0, 0, 7);
    
//    parameters->add(randomAdd_Param.set("Random Addition", 0, -.5, .5));
//    parameters->add(scale_Param.set("Scale", 1, 0, 2));
//    parameters->add(offset_Param.set("Offset", 0, -1, 1));
//    parameters->add(pow_Param.set("Pow", 0, -40, 40));
//    parameters->add(biPow_Param.set("Bi Pow", 0, -40, 40));
//    parameters->add(quant_Param.set("Quantization", 255, 1, 255));
//    parameters->add(pulseWidth_Param.set("Pulse Width", 1, 0, 1));
//    parameters->add(skew_Param.set("Skew", 0, -1, 1));
//    parameters->add(amplitude_Param.set("Fader", 1, 0, 1));
//    parameters->add(invert_Param.set("Invert", 0, 0, 1));
//    ofParameterGroup waveDropDown;
//    waveDropDown.setName("Wave Select");
//    ofParameter<string> tempStrParam("Options", "sin-|-cos-|-tri-|-square-|-saw-|-inverted saw-|-rand1-|-rand2");
//    waveDropDown.add(tempStrParam);
//    waveDropDown.add(waveSelect_Param.set("Wave Select", 0, 0, 7));
//    parameters->add(waveDropDown);
    
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
    phaseOffsetBuffer.allocate();
    phaseOffsetBuffer.bind(GL_TEXTURE_BUFFER);
    phaseOffsetBuffer.setData(vector<float>(width + height, phaseOffset[0].get()[0]), GL_STREAM_DRAW);
    phaseOffsetTexture.allocateAsBufferTexture(phaseOffsetBuffer, GL_R32F);

    //Random Addition
    randomAdditionBuffer.allocate();
    randomAdditionBuffer.bind(GL_TEXTURE_BUFFER);
    randomAdditionBuffer.setData(vector<float>(width + height, randomAddition[0].get()[0]), GL_STREAM_DRAW);
    randomAdditionTexture.allocateAsBufferTexture(randomAdditionBuffer, GL_R32F);
    
    //Scale
    scaleBuffer.allocate();
    scaleBuffer.bind(GL_TEXTURE_BUFFER);
    scaleBuffer.setData(vector<float>(width + height, scale[0].get()[0]), GL_STREAM_DRAW);
    scaleTexture.allocateAsBufferTexture(scaleBuffer, GL_R32F);
    
    //Offset
    offsetBuffer.allocate();
    offsetBuffer.bind(GL_TEXTURE_BUFFER);
    offsetBuffer.setData(vector<float>(width + height, offset[0].get()[0]), GL_STREAM_DRAW);
    offsetTexture.allocateAsBufferTexture(offsetBuffer, GL_R32F);
    
    //Pow
    powBuffer.allocate();
    powBuffer.bind(GL_TEXTURE_BUFFER);
    powBuffer.setData(vector<float>(width + height, pow[0].get()[0]), GL_STREAM_DRAW);
    powTexture.allocateAsBufferTexture(powBuffer, GL_R32F);
    
    //Bipow
    bipowBuffer.allocate();
    bipowBuffer.bind(GL_TEXTURE_BUFFER);
    bipowBuffer.setData(vector<float>(width + height, bipow[0].get()[0]), GL_STREAM_DRAW);
    bipowTexture.allocateAsBufferTexture(bipowBuffer, GL_R32F);
    
    //Quantization
    quantizationBuffer.allocate();
    quantizationBuffer.bind(GL_TEXTURE_BUFFER);
    quantizationBuffer.setData(vector<float>(width + height, quantization[0].get()[0]), GL_STREAM_DRAW);
    quantizationTexture.allocateAsBufferTexture(quantizationBuffer, GL_R32UI);
    
    //Pulse Width
    pulseWidthBuffer.allocate();
    pulseWidthBuffer.bind(GL_TEXTURE_BUFFER);
    pulseWidthBuffer.setData(vector<float>(width + height, pulseWidth[0].get()[0]), GL_STREAM_DRAW);
    pulseWidthTexture.allocateAsBufferTexture(pulseWidthBuffer, GL_R32F);
    
    //Skew
    skewBuffer.allocate();
    skewBuffer.bind(GL_TEXTURE_BUFFER);
    skewBuffer.setData(vector<float>(width + height, skew[0].get()[0]), GL_STREAM_DRAW);
    skewTexture.allocateAsBufferTexture(skewBuffer, GL_R32F);
    
    //Fader
    faderBuffer.allocate();
    faderBuffer.bind(GL_TEXTURE_BUFFER);
    faderBuffer.setData(vector<float>(width + height, fader[0].get()[0]), GL_STREAM_DRAW);
    faderTexture.allocateAsBufferTexture(faderBuffer, GL_R32F);
    
    //Invert
    invertBuffer.allocate();
    invertBuffer.bind(GL_TEXTURE_BUFFER);
    invertBuffer.setData(vector<float>(width + height, invert[0].get()[0]), GL_STREAM_DRAW);
    invertTexture.allocateAsBufferTexture(invertBuffer, GL_R32F);
    
    
    //Waveform
    waveformBuffer.allocate();
    waveformBuffer.bind(GL_TEXTURE_BUFFER);
    waveformBuffer.setData(vector<float>(width + height, waveform[0].get()[0]), GL_STREAM_DRAW);
    waveformTexture.allocateAsBufferTexture(waveformBuffer, GL_R32F);
    
    
    //LoadShader
    bool b = true;
    reloadShader(b);
    
    phasorIn.addListener(this, &oscillatorTexture::newPhasorIn);
    
    phaseOffset[0].addListener(this, &oscillatorTexture::phaseOffsetListener);
    phaseOffset[1].addListener(this, &oscillatorTexture::phaseOffsetListener);
    randomAddition[0].addListener(this, &oscillatorTexture::randomAdditionListener);
    randomAddition[1].addListener(this, &oscillatorTexture::randomAdditionListener);
    scale[0].addListener(this, &oscillatorTexture::scaleListener);
    scale[1].addListener(this, &oscillatorTexture::scaleListener);
    offset[0].addListener(this, &oscillatorTexture::offsetListener);
    offset[1].addListener(this, &oscillatorTexture::offsetListener);
    pow[0].addListener(this, &oscillatorTexture::powListener);
    pow[1].addListener(this, &oscillatorTexture::powListener);
    bipow[0].addListener(this, &oscillatorTexture::bipowListener);
    bipow[1].addListener(this, &oscillatorTexture::bipowListener);
    quantization[0].addListener(this, &oscillatorTexture::quantizationListener);
    quantization[1].addListener(this, &oscillatorTexture::quantizationListener);
    pulseWidth[0].addListener(this, &oscillatorTexture::pulseWidthListener);
    pulseWidth[1].addListener(this, &oscillatorTexture::pulseWidthListener);
    skew[0].addListener(this, &oscillatorTexture::skewListener);
    skew[1].addListener(this, &oscillatorTexture::skewListener);
    fader[0].addListener(this, &oscillatorTexture::faderListener);
    fader[1].addListener(this, &oscillatorTexture::faderListener);
    invert[0].addListener(this, &oscillatorTexture::invertListener);
    invert[1].addListener(this, &oscillatorTexture::invertListener);
    
    waveform[0].addListener(this, &oscillatorTexture::waveformListener);
    waveform[1].addListener(this, &oscillatorTexture::waveformListener);
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::red, pos);
    
    ofAddListener(parameters->parameterChangedE(), this,  &oscillatorTexture::parameterChanged);
}

void oscillatorTexture::parameterChanged(ofAbstractParameter &p){
    
}

void oscillatorTexture::reloadShader(bool &b){
    shader.load("noise.vert", "noise.frag");
    shader.begin();
    shader.setUniform1i("width", width);
    shader.setUniformTexture("xIndexs", xIndexTexture, 0);
    shader.setUniformTexture("yIndexs", yIndexTexture, 1);
    shader.setUniformTexture("phaseOffset", phaseOffsetTexture, 2);
    shader.setUniformTexture("randomAddition", randomAdditionTexture, 3);
    shader.setUniformTexture("scale", scaleTexture, 4);
    shader.setUniformTexture("offset", offsetTexture, 5);
    shader.setUniformTexture("pow_", powTexture, 6);
    shader.setUniformTexture("bipow", bipowTexture, 7);
    shader.setUniformTexture("quantization", quantizationTexture, 8);
    shader.setUniformTexture("pulseWidth", pulseWidthTexture, 9);
    shader.setUniformTexture("skew", skewTexture, 10);
    shader.setUniformTexture("fader", faderTexture, 11);
    shader.setUniformTexture("invert_", invertTexture, 12);
    shader.setUniformTexture("waveform", waveformTexture, 13);
    shader.end();
}

ofTexture& oscillatorTexture::computeBank(float phasor){

    fbo.begin();
    shader.begin();
    shader.setUniform1f("phase", phasor);
    shader.setUniform1f("time", ofGetElapsedTimef());
    ofDrawRectangle(0, 0, width, height);
    shader.end();
    fbo.end();

    
    return fbo.getTexture();
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

void oscillatorTexture::phaseOffsetListener(vector<float> &vf){
    if(&vf == &phaseOffset[0].get()){
        if(vf.size() == 1){
            phaseOffsetBuffer.updateData(0, vector<float>(width, vf[0]));
        }else if(vf.size() == width){
            phaseOffsetBuffer.updateData(0, vf);
        }
    }
    if(&vf == &phaseOffset[1].get()){
        if(vf.size() == 1){
            phaseOffsetBuffer.updateData(width*4, vector<float>(height, vf[0]));
        }else if(vf.size() == height){
            phaseOffsetBuffer.updateData(width*4, vf);
        }
    }
}

void oscillatorTexture::randomAdditionListener(vector<float> &vf){
    if(&vf == &randomAddition[0].get()){
        if(vf.size() == 1){
            randomAdditionBuffer.updateData(0, vector<float>(width, vf[0]));
        }else if(vf.size() == width){
            randomAdditionBuffer.updateData(0, vf);
        }
    }
    if(&vf == &randomAddition[1].get()){
        if(vf.size() == 1){
            randomAdditionBuffer.updateData(width*4, vector<float>(height, vf[0]));
        }else if(vf.size() == height){
            randomAdditionBuffer.updateData(width*4, vf);
        }
    }
}


void oscillatorTexture::scaleListener(vector<float> &vf){
    if(&vf == &scale[0].get()){
        if(vf.size() == 1){
            scaleBuffer.updateData(0, vector<float>(width, vf[0]));
        }else if(vf.size() == width){
            scaleBuffer.updateData(0, vf);
        }
    }
    if(&vf == &scale[1].get()){
        if(vf.size() == 1){
            scaleBuffer.updateData(width*4, vector<float>(height, vf[0]));
        }else if(vf.size() == height){
            scaleBuffer.updateData(width*4, vf);
        }
    }
}

void oscillatorTexture::offsetListener(vector<float> &vf){
    if(&vf == &offset[0].get()){
        if(vf.size() == 1){
            offsetBuffer.updateData(0, vector<float>(width, vf[0]));
        }else if(vf.size() == width){
            offsetBuffer.updateData(0, vf);
        }
    }
    if(&vf == &offset[1].get()){
        if(vf.size() == 1){
            offsetBuffer.updateData(width*4, vector<float>(height, vf[0]));
        }else if(vf.size() == height){
            offsetBuffer.updateData(width*4, vf);
        }
    }
}

void oscillatorTexture::powListener(vector<float> &vf){
    if(&vf == &pow[0].get()){
        if(vf.size() == 1){
            powBuffer.updateData(0, vector<float>(width, vf[0]));
        }else if(vf.size() == width){
            powBuffer.updateData(0, vf);
        }
    }
    if(&vf == &pow[1].get()){
        if(vf.size() == 1){
            powBuffer.updateData(width*4, vector<float>(height, vf[0]));
        }else if(vf.size() == height){
            powBuffer.updateData(width*4, vf);
        }
    }
}

void oscillatorTexture::bipowListener(vector<float> &vf){
    if(&vf == &bipow[0].get()){
        if(vf.size() == 1){
            bipowBuffer.updateData(0, vector<float>(width, vf[0]));
        }else if(vf.size() == width){
            bipowBuffer.updateData(0, vf);
        }
    }
    if(&vf == &bipow[1].get()){
        if(vf.size() == 1){
            bipowBuffer.updateData(width*4, vector<float>(height, vf[0]));
        }else if(vf.size() == height){
            bipowBuffer.updateData(width*4, vf);
        }
    }
}

void oscillatorTexture::quantizationListener(vector<int> &vf){
    if(&vf == &quantization[0].get()){
        if(vf.size() == 1){
            quantizationBuffer.updateData(0, vector<int>(width, vf[0]));
        }else if(vf.size() == width){
            quantizationBuffer.updateData(0, vf);
        }
    }
    if(&vf == &quantization[1].get()){
        if(vf.size() == 1){
            quantizationBuffer.updateData(width*4, vector<int>(height, vf[0]));
        }else if(vf.size() == height){
            quantizationBuffer.updateData(width*4, vf);
        }
    }
}

void oscillatorTexture::pulseWidthListener(vector<float> &vf){
    if(&vf == &pulseWidth[0].get()){
        if(vf.size() == 1){
            pulseWidthBuffer.updateData(0, vector<float>(width, vf[0]));
        }else if(vf.size() == width){
            pulseWidthBuffer.updateData(0, vf);
        }
    }
    if(&vf == &pulseWidth[1].get()){
        if(vf.size() == 1){
            pulseWidthBuffer.updateData(width*4, vector<float>(height, vf[0]));
        }else if(vf.size() == height){
            pulseWidthBuffer.updateData(width*4, vf);
        }
    }
}

void oscillatorTexture::skewListener(vector<float> &vf){
    if(&vf == &skew[0].get()){
        if(vf.size() == 1){
            skewBuffer.updateData(0, vector<float>(width, vf[0]));
        }else if(vf.size() == width){
            skewBuffer.updateData(0, vf);
        }
    }
    if(&vf == &skew[1].get()){
        if(vf.size() == 1){
            skewBuffer.updateData(width*4, vector<float>(height, vf[0]));
        }else if(vf.size() == height){
            skewBuffer.updateData(width*4, vf);
        }
    }
}

void oscillatorTexture::faderListener(vector<float> &vf){
    if(&vf == &fader[0].get()){
        if(vf.size() == 1){
            faderBuffer.updateData(0, vector<float>(width, vf[0]));
        }else if(vf.size() == width){
            faderBuffer.updateData(0, vf);
        }
    }
    if(&vf == &fader[1].get()){
        if(vf.size() == 1){
            faderBuffer.updateData(width*4, vector<float>(height, vf[0]));
        }else if(vf.size() == height){
            faderBuffer.updateData(width*4, vf);
        }
    }
}

void oscillatorTexture::invertListener(vector<float> &vf){
    if(&vf == &invert[0].get()){
        if(vf.size() == 1){
            invertBuffer.updateData(0, vector<float>(width, vf[0]));
        }else if(vf.size() == width){
            invertBuffer.updateData(0, vf);
        }
    }
    if(&vf == &invert[1].get()){
        if(vf.size() == 1){
            invertBuffer.updateData(width*4, vector<float>(height, vf[0]));
        }else if(vf.size() == height){
            invertBuffer.updateData(width*4, vf);
        }
    }
}

void oscillatorTexture::waveformListener(vector<float> &vf){
    if(&vf == &waveform[0].get()){
        if(vf.size() == 1){
            waveformBuffer.updateData(0, vector<float>(width, vf[0]));
        }else if(vf.size() == width){
            waveformBuffer.updateData(0, vf);
        }
    }
    if(&vf == &waveform[1].get()){
        if(vf.size() == 1){
            waveformBuffer.updateData(width*4, vector<float>(height, vf[0]));
        }else if(vf.size() == height){
            waveformBuffer.updateData(width*4, vf);
        }
    }
}


