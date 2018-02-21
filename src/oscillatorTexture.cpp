//
//  oscillatorTexture.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 24/01/2018.
//
//

#include "oscillatorTexture.h"
#include "parametersControl.h"
#include <random>

oscillatorTexture::oscillatorTexture(int bankId, int xSize, int ySize, ofPoint pos){
    resources = &sharedResources::getInstance();
    
    parameters = new ofParameterGroup();
    
    width = xSize;
    height = ySize;
    
    fbo.allocate(width, height, GL_RGBA32F);
    fbo.begin();
    ofClear(255, 255, 255, 0);
    fbo.end();
    fboBuffer.allocate(width, height, GL_RGBA16);
    fboBuffer.begin();
    ofClear(255, 255, 255, 0);
    fboBuffer.end();
    fbo.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    randomInfoTexture.allocate(width, height, GL_RGBA32F);
    randomInfoFbo.allocate(width, height, GL_RGBA32F);
    randomInfoFbo.begin();
    ofClear(255, 255, 255, 0);
    randomInfoFbo.end();

    
    parameters->setName("oscillatorTexture " + ofToString(bankId));
    
    parameters->add(reloadShaderParam.set("Reload Shader", false));
//    reloadShaderParam.addListener(this, &oscillatorTexture::reloadShader);
    
//    ofParameter<string> strlbl;
//    parameters->add(strlbl.set(dimensionStr + " Dim_label", " "));
    
    auto setAndBindXYParamsVecFloat = [this](ofParameter<vector<float>> *p, string name, float val, float min, float max) -> void{
        parameters->add(p[0].set(name + " X", vector<float>(1, val), vector<float>(1, min), vector<float>(1, max)));
        parameters->add(p[1].set(name + " Y", vector<float>(1, val), vector<float>(1, min), vector<float>(1, max)));
    };
    
    auto setAndBindXYParamsVecInt = [this](ofParameter<vector<int>> *p, string name, int val, int min, int max) -> void{
        parameters->add(p[0].set(name + " X", vector<int>(1, val), vector<int>(1, min), vector<int>(1, max)));
        parameters->add(p[1].set(name + " Y", vector<int>(1, val), vector<int>(1, min), vector<int>(1, max)));
    };
    
    parameters->add(phasorIn.set("Phasor In", 0, 0, 1));
    
    parameters->add(indexNumWaves[0].set("Num Waves X", vector<float>(1, 1), vector<float>(1, 0), vector<float>(1, width)));
    parameters->add(indexNumWaves[1].set("Num Waves Y", vector<float>(1, 1), vector<float>(1, 0), vector<float>(1, height)));
    setAndBindXYParamsVecFloat(indexInvert, "Index Invert", 1, -1, 1);
    parameters->add(indexSymmetry[0].set("Symmetry X", vector<int>(1, 0), vector<int>(1, 0), vector<int>(1, width/2)));
    parameters->add(indexSymmetry[1].set("Symmetry Y", vector<int>(1, 0), vector<int>(1, 0), vector<int>(1, height/2)));
    setAndBindXYParamsVecFloat(indexRandom, "Index Random", 0, 0, 1);
    parameters->add(indexOffset[0].set("Index Offset X", vector<float>(1, 0), vector<float>(1, -width/2), vector<float>(1, width/2)));
    parameters->add(indexOffset[1].set("Index Offset Y", vector<float>(1, 0), vector<float>(1, -height/2), vector<float>(1, height/2)));
    parameters->add(indexQuantization[0].set("Index Quatization X", vector<int>(1, width), vector<int>(1, 1), vector<int>(1, width)));
    parameters->add(indexQuantization[1].set("Index Quatization Y", vector<int>(1, height), vector<int>(1, 1), vector<int>(1, height)));
    setAndBindXYParamsVecFloat(indexCombination, "Index Combination", 0, 0, 1);
    parameters->add(indexModulo[0].set("Index Modulo X", vector<int>(1, width), vector<int>(1, 1), vector<int>(1, width)));
    parameters->add(indexModulo[1].set("Index Modulo Y", vector<int>(1, height), vector<int>(1, 1), vector<int>(1, height)));

    
    setAndBindXYParamsVecFloat(phaseOffset, "Phase Offset", 0, 0, 1);
    setAndBindXYParamsVecFloat(randomAddition, "Rnd Add", 0, -1, 1);
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
    

    ofParameterGroup waveDropDown;
    waveDropDown.setName("Wave Select");
    ofParameter<string> tempStrParam("Options", "sin-|-cos-|-tri-|-square-|-saw-|-inverted saw-|-rand1-|-rand2");
    waveDropDown.add(tempStrParam);
    waveDropDown.add(waveSelect_Param.set("Wave Select", 0, 0, 7));
    parameters->add(waveDropDown);
    
    parameters->add(oscillatorOut.set("Oscillator Out", nullptr));
    
    
    
    
    //TBOs
    
    //NumWaves
    indexNumWavesBuffer.allocate();
    indexNumWavesBuffer.bind(GL_TEXTURE_BUFFER);
    indexNumWavesBuffer.setData(vector<float>(width + height, indexNumWaves[0].get()[0]), GL_STREAM_DRAW);
    indexNumWavesTexture.allocateAsBufferTexture(indexNumWavesBuffer, GL_R32F);
    
    //Invert
    indexInvertBuffer.allocate();
    indexInvertBuffer.bind(GL_TEXTURE_BUFFER);
    indexInvertBuffer.setData(vector<float>(width + height, indexInvert[0].get()[0]), GL_STREAM_DRAW);
    indexInvertTexture.allocateAsBufferTexture(indexInvertBuffer, GL_R32F);
    
    //Symmetry
    indexSymmetryBuffer.allocate();
    indexSymmetryBuffer.bind(GL_TEXTURE_BUFFER);
    indexSymmetryBuffer.setData(vector<int>(width + height, indexSymmetry[0].get()[0]), GL_STREAM_DRAW);
    indexSymmetryTexture.allocateAsBufferTexture(indexSymmetryBuffer, GL_R32UI);
    
    //Random
    indexRandomBuffer.allocate();
    indexRandomBuffer.bind(GL_TEXTURE_BUFFER);
    indexRandomBuffer.setData(vector<float>(width + height, indexRandom[0].get()[0]), GL_STREAM_DRAW);
    indexRandomTexture.allocateAsBufferTexture(indexRandomBuffer, GL_R32F);
    
    //Offset
    indexOffsetBuffer.allocate();
    indexOffsetBuffer.bind(GL_TEXTURE_BUFFER);
    indexOffsetBuffer.setData(vector<float>(width + height, indexOffset[0].get()[0]), GL_STREAM_DRAW);
    indexOffsetTexture.allocateAsBufferTexture(indexOffsetBuffer, GL_R32F);
    
    //Quantization
    vector<int> xQuantVec(width, indexQuantization[0].get()[0]);
    vector<int> yQuantVec(height, indexQuantization[1].get()[0]);
    xQuantVec.insert(xQuantVec.end(), yQuantVec.begin(), yQuantVec.end());
    indexQuantizationBuffer.allocate();
    indexQuantizationBuffer.bind(GL_TEXTURE_BUFFER);
    indexQuantizationBuffer.setData(xQuantVec, GL_STREAM_DRAW);
    indexQuantizationTexture.allocateAsBufferTexture(indexQuantizationBuffer, GL_R32UI);
    
    //Combination
    indexCombinationBuffer.allocate();
    indexCombinationBuffer.bind(GL_TEXTURE_BUFFER);
    indexCombinationBuffer.setData(vector<float>(width + height, indexCombination[0].get()[0]), GL_STREAM_DRAW);
    indexCombinationTexture.allocateAsBufferTexture(indexCombinationBuffer, GL_R32F);
    
    //Modulo
    vector<int> xModVec(width, indexModulo[0].get()[0]);
    vector<int> yModtVec(height, indexModulo[1].get()[0]);
    xModVec.insert(xModVec.end(), yModtVec.begin(), yModtVec.end());
    indexModuloBuffer.allocate();
    indexModuloBuffer.bind(GL_TEXTURE_BUFFER);
    indexModuloBuffer.setData(xModVec, GL_STREAM_DRAW);
    indexModuloTexture.allocateAsBufferTexture(indexModuloBuffer, GL_R32UI);
    
    
    
    //IndexRandomValues
    indexRandomValuesBuffer.allocate();
    indexRandomValuesBuffer.bind(GL_TEXTURE_BUFFER);
    indexRandomValuesBuffer.setData(newRandomValuesVector(), GL_STREAM_DRAW);
    indexRandomValuesTexture.allocateAsBufferTexture(indexRandomValuesBuffer, GL_R32F);
    
    
    
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
    
    indexNumWaves[0].addListener(this, &oscillatorTexture::indexNumWavesListener);
    indexNumWaves[1].addListener(this, &oscillatorTexture::indexNumWavesListener);
    indexInvert[0].addListener(this, &oscillatorTexture::indexInvertListener);
    indexInvert[1].addListener(this, &oscillatorTexture::indexInvertListener);
    indexSymmetry[0].addListener(this, &oscillatorTexture::indexSymmetryListener);
    indexSymmetry[1].addListener(this, &oscillatorTexture::indexSymmetryListener);
    indexRandom[0].addListener(this, &oscillatorTexture::indexRandomListener);
    indexRandom[1].addListener(this, &oscillatorTexture::indexRandomListener);
    indexOffset[0].addListener(this, &oscillatorTexture::indexOffsetListener);
    indexOffset[1].addListener(this, &oscillatorTexture::indexOffsetListener);
    indexQuantization[0].addListener(this, &oscillatorTexture::indexQuantizationListener);
    indexQuantization[1].addListener(this, &oscillatorTexture::indexQuantizationListener);
    indexCombination[0].addListener(this, &oscillatorTexture::indexCombinationListener);
    indexCombination[1].addListener(this, &oscillatorTexture::indexCombinationListener);
    indexModulo[0].addListener(this, &oscillatorTexture::indexModuloListener);
    indexModulo[1].addListener(this, &oscillatorTexture::indexModuloListener);
    
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
    waveSelect_Param.addListener(this, &oscillatorTexture::newWaveSelectParam);

    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::red, pos);
    
    ofAddListener(parameters->parameterChangedE(), this,  &oscillatorTexture::parameterChanged);
}

void oscillatorTexture::parameterChanged(ofAbstractParameter &p){
    
}

void oscillatorTexture::reloadShader(bool &b){
    shaderOscillator.load("Shaders/oscillator.vert", "Shaders/oscillator.frag");
    shaderOscillator.begin();
    
    shaderOscillator.setUniformTexture("indexNumWaves", indexNumWavesTexture, resources->getNextAvailableShaderTextureLocation());
    shaderOscillator.setUniformTexture("indexInvert", indexInvertTexture, resources->getNextAvailableShaderTextureLocation());
    shaderOscillator.setUniformTexture("indexSymmetry", indexSymmetryTexture, resources->getNextAvailableShaderTextureLocation());
    shaderOscillator.setUniformTexture("indexRandom", indexRandomTexture, resources->getNextAvailableShaderTextureLocation());
    shaderOscillator.setUniformTexture("indexOffset", indexOffsetTexture, resources->getNextAvailableShaderTextureLocation());
    shaderOscillator.setUniformTexture("indexQuantization", indexQuantizationTexture, resources->getNextAvailableShaderTextureLocation());
    shaderOscillator.setUniformTexture("indexCombination", indexCombinationTexture, resources->getNextAvailableShaderTextureLocation());
    shaderOscillator.setUniformTexture("indexModulo", indexModuloTexture, resources->getNextAvailableShaderTextureLocation());
    shaderOscillator.setUniformTexture("indexRandomValues", indexRandomValuesTexture, resources->getNextAvailableShaderTextureLocation());
    
    shaderOscillator.setUniformTexture("phaseOffset", phaseOffsetTexture, resources->getNextAvailableShaderTextureLocation());
    shaderOscillator.setUniformTexture("pulseWidth", pulseWidthTexture, resources->getNextAvailableShaderTextureLocation());
    shaderOscillator.setUniformTexture("skew", skewTexture, resources->getNextAvailableShaderTextureLocation());
    shaderOscillator.setUniformTexture("waveform", waveformTexture, resources->getNextAvailableShaderTextureLocation());
    shaderOscillator.end();
    
    shaderScaling.load("Shaders/scaling.vert", "Shaders/scaling.frag");
    shaderScaling.begin();
    shaderScaling.setUniformTexture("randomAddition", randomAdditionTexture, resources->getNextAvailableShaderTextureLocation());
    shaderScaling.setUniformTexture("scale", scaleTexture, resources->getNextAvailableShaderTextureLocation());
    shaderScaling.setUniformTexture("offset", offsetTexture, resources->getNextAvailableShaderTextureLocation());
    shaderScaling.setUniformTexture("pow_", powTexture, resources->getNextAvailableShaderTextureLocation());
    shaderScaling.setUniformTexture("bipow", bipowTexture, resources->getNextAvailableShaderTextureLocation());
    shaderScaling.setUniformTexture("quantization", quantizationTexture, resources->getNextAvailableShaderTextureLocation());
    shaderScaling.setUniformTexture("fader", faderTexture, resources->getNextAvailableShaderTextureLocation());
    shaderScaling.setUniformTexture("invert_", invertTexture, resources->getNextAvailableShaderTextureLocation());
    shaderScaling.end();
    
    randomInfoOscillatorShaderTextureLocation = resources->getNextAvailableShaderTextureLocation();
    randomInfoScalingShaderTextureLocation = resources->getNextAvailableShaderTextureLocation();
}

ofTexture& oscillatorTexture::computeBank(float phasor){
    ofPushStyle();
    ofSetColor(255, 255);
    fboBuffer.begin();
    ofClear(255, 255, 255, 255);
    fboBuffer.end();
    fboBuffer.begin();
    shaderOscillator.begin();
    shaderOscillator.setUniform1f("phase", phasor);
    shaderOscillator.setUniform1f("time", ofGetElapsedTimef());
    shaderOscillator.setUniformTexture("randomInfo", randomInfoTexture, randomInfoOscillatorShaderTextureLocation);
    ofDrawRectangle(0, 0, width, height);
    shaderOscillator.end();
    fboBuffer.end();
    
    randomInfoFbo.begin();
    ofClear(0, 0, 0, 255);
    fboBuffer.draw(0,0);
    randomInfoFbo.end();
    randomInfoTexture = randomInfoFbo.getTexture();
    
    fbo.begin();
    shaderScaling.begin();
    shaderScaling.setUniform1f("phase", phasor);
    shaderScaling.setUniform1f("time", ofGetElapsedTimef());
    shaderScaling.setUniformTexture("randomInfo", randomInfoTexture, randomInfoScalingShaderTextureLocation);
    ofDrawRectangle(0, 0, width, height);
    shaderScaling.end();
    fbo.end();
    ofPopStyle();
    
    return fbo.getTexture();
}

void oscillatorTexture::newPhasorIn(float &f){
    parameters->get("Oscillator Out").cast<ofTexture*>() = &computeBank(f);
}


vector<float> oscillatorTexture::newRandomValuesVector(){
    vector<float> randomValuesVecX(width, 0);
    vector<float> randomValuesVecY(height, 0);
    for(int i = 0; i < randomValuesVecX.size(); i++){
        randomValuesVecX[i] = (int)-randomValuesVecX.size()/2 + i;
    }
    for(int i = 0; i < randomValuesVecY.size(); i++){
        randomValuesVecY[i] = (int)-randomValuesVecY.size()/2 + i;
    }
    
    mt19937 g(static_cast<uint32_t>(time(0)));
    shuffle(randomValuesVecX.begin(), randomValuesVecX.end(), g);
    shuffle(randomValuesVecY.begin(), randomValuesVecY.end(), g);
    
    randomValuesVecX.insert(randomValuesVecX.end(), randomValuesVecY.begin(), randomValuesVecY.end());
    
    return randomValuesVecX;
}


#pragma mark Index Listeners

void oscillatorTexture::indexNumWavesListener(vector<float> &vf){
    if(&vf == &indexNumWaves[0].get()){
        if(vf.size() == 1){
            indexNumWavesBuffer.updateData(0, vector<float>(width, vf[0]));
        }else if(vf.size() == width){
            indexNumWavesBuffer.updateData(0, vf);
        }
    }
    if(&vf == &indexNumWaves[1].get()){
        if(vf.size() == 1){
            indexNumWavesBuffer.updateData(width*4, vector<float>(height, vf[0]));
        }else if(vf.size() == height){
            indexNumWavesBuffer.updateData(width*4, vf);
        }
    }
}

void oscillatorTexture::indexInvertListener(vector<float> &vf){
    if(&vf == &indexInvert[0].get()){
        if(vf.size() == 1){
            indexInvertBuffer.updateData(0, vector<float>(width, vf[0]));
        }else if(vf.size() == width){
            indexInvertBuffer.updateData(0, vf);
        }
    }
    if(&vf == &indexInvert[1].get()){
        if(vf.size() == 1){
            indexInvertBuffer.updateData(width*4, vector<float>(height, vf[0]));
        }else if(vf.size() == height){
            indexInvertBuffer.updateData(width*4, vf);
        }
    }
}

void oscillatorTexture::indexSymmetryListener(vector<int> &vi){
    if(&vi == &indexSymmetry[0].get()){
        if(vi.size() == 1){
            indexSymmetryBuffer.updateData(0, vector<int>(width, vi[0]));
        }else if(vi.size() == width){
            indexSymmetryBuffer.updateData(0, vi);
        }
    }
    if(&vi == &indexSymmetry[1].get()){
        if(vi.size() == 1){
            indexSymmetryBuffer.updateData(width*4, vector<int>(height, vi[0]));
        }else if(vi.size() == height){
            indexSymmetryBuffer.updateData(width*4, vi);
        }
    }
}

void oscillatorTexture::indexRandomListener(vector<float> &vf){
    if(accumulate(vf.begin(), vf.end(), 0.0) == 0.0){
        indexRandomValuesBuffer.updateData(0, newRandomValuesVector());
    }
    if(&vf == &indexRandom[0].get()){
        if(vf.size() == 1){
            indexRandomBuffer.updateData(0, vector<float>(width, vf[0]));
        }else if(vf.size() == width){
            indexRandomBuffer.updateData(0, vf);
        }
    }
    if(&vf == &indexRandom[1].get()){
        if(vf.size() == 1){
            indexRandomBuffer.updateData(width*4, vector<float>(height, vf[0]));
        }else if(vf.size() == height){
            indexRandomBuffer.updateData(width*4, vf);
        }
    }
}

void oscillatorTexture::indexOffsetListener(vector<float> &vf){
    if(&vf == &indexOffset[0].get()){
        if(vf.size() == 1){
            indexOffsetBuffer.updateData(0, vector<float>(width, vf[0]));
        }else if(vf.size() == width){
            indexOffsetBuffer.updateData(0, vf);
        }
    }
    if(&vf == &indexOffset[1].get()){
        if(vf.size() == 1){
            indexOffsetBuffer.updateData(width*4, vector<float>(height, vf[0]));
        }else if(vf.size() == height){
            indexOffsetBuffer.updateData(width*4, vf);
        }
    }
}

void oscillatorTexture::indexQuantizationListener(vector<int> &vi){
    if(&vi == &indexQuantization[0].get()){
        if(vi.size() == 1){
            indexQuantizationBuffer.updateData(0, vector<int>(width, vi[0]));
        }else if(vi.size() == width){
            indexQuantizationBuffer.updateData(0, vi);
        }
    }
    if(&vi == &indexQuantization[1].get()){
        if(vi.size() == 1){
            indexQuantizationBuffer.updateData(width*4, vector<int>(height, vi[0]));
        }else if(vi.size() == height){
            indexQuantizationBuffer.updateData(width*4, vi);
        }
    }
}

void oscillatorTexture::indexCombinationListener(vector<float> &vf){
    if(&vf == &indexCombination[0].get()){
        if(vf.size() == 1){
            indexCombinationBuffer.updateData(0, vector<float>(width, vf[0]));
        }else if(vf.size() == width){
            indexCombinationBuffer.updateData(0, vf);
        }
    }
    if(&vf == &indexCombination[1].get()){
        if(vf.size() == 1){
            indexCombinationBuffer.updateData(width*4, vector<float>(height, vf[0]));
        }else if(vf.size() == height){
            indexCombinationBuffer.updateData(width*4, vf);
        }
    }
}

void oscillatorTexture::indexModuloListener(vector<int> &vi){
    if(&vi == &indexModulo[0].get()){
        if(vi.size() == 1){
            indexModuloBuffer.updateData(0, vector<int>(width, vi[0]));
        }else if(vi.size() == width){
            indexModuloBuffer.updateData(0, vi);
        }
    }
    if(&vi == &indexModulo[1].get()){
        if(vi.size() == 1){
            indexModuloBuffer.updateData(width*4, vector<int>(height, vi[0]));
        }else if(vi.size() == height){
            indexModuloBuffer.updateData(width*4, vi);
        }
    }
}

#pragma mark Oscillator listeners

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

void oscillatorTexture::quantizationListener(vector<int> &vi){
    if(&vi == &quantization[0].get()){
        if(vi.size() == 1){
            quantizationBuffer.updateData(0, vector<int>(width, vi[0]));
        }else if(vi.size() == width){
            quantizationBuffer.updateData(0, vi);
        }
    }
    if(&vi == &quantization[1].get()){
        if(vi.size() == 1){
            quantizationBuffer.updateData(width*4, vector<int>(height, vi[0]));
        }else if(vi.size() == height){
            quantizationBuffer.updateData(width*4, vi);
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

void oscillatorTexture::newWaveSelectParam(int &i){
    waveform[0] = vector<float>(1, i);
}


