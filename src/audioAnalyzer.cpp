//
//  audioAnalyzer.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 07/09/2017.
//
//

#include "audioAnalyzer.h"
#include "parametersControl.h"

audioAnalyzer::audioAnalyzer(int nChannels){
    parameters = new ofParameterGroup();
    parameters->setName("audioAnalyzer");
    
    parameters->add(gain.set("Gain", 1, 0, 10));
    parameters->add(smoothing.set("Smoothing", 0, 0, 1));
    
    ampFloatStore.resize(nChannels, 0);
    oldValues.resize(nChannels, 0);
    
    amplitudeOuts.resize(nChannels);
    for(int i = 0 ; i < amplitudeOuts.size() ; i++){
        parameters->add(amplitudeOuts[i].set("Amplitude " + ofToString(i), 0, 0, 1));
    }
//    fftOuts.resize(nChannels);
//    for(int i = 0 ; i < fftOuts.size() ; i++){
//        parameters->add(fftOuts[i].set("Fft " + ofToString(i), {}));
//    }
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::green);
    
    bands.setup();
    
    ofAddListener(ofEvents().update, this, &audioAnalyzer::update);
}

void audioAnalyzer::audioIn(float *input, int bufferSize, int nChannels){
    float sum = 0.0f;
    float actual = 0.0f;
    for (int i = 0; i < amplitudeOuts.size(); i++)
    {
        sum = 0;
        for (int j=0; j < bufferSize; j++)
        {
            actual = abs(input[(j * nChannels) + i] * gain);
            sum = sum + actual;
        }
        sum /= (float)bufferSize;
        sum = ofClamp(sum, 0, 1);
        ampFloatStore[i] = (1-smoothing)*(sum) + (smoothing * oldValues[i]);
        oldValues[i] = ampFloatStore[i];
    }
}

void audioAnalyzer::update(ofEventArgs &args){
    for(int i = 0 ; i < amplitudeOuts.size() ; i++){
        parameters->getFloat(amplitudeOuts[i].getName()) = ampFloatStore[i];
    }
}
