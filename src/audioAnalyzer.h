//
//  audioAnalyzer.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 07/09/2017.
//
//

#ifndef audioAnalyzer_h
#define audioAnalyzer_h

#include "ofMain.h"
#include "ofxAubio.h"

class audioAnalyzer{
public:
    audioAnalyzer(int nChannels);
    ~audioAnalyzer(){};
    
    void audioIn(float* input, int bufferSize, int nChannels);
private:
    
    void update(ofEventArgs &args);
    
    ofParameterGroup *parameters;
    ofParameter<float>  gain;
    ofParameter<float>  smoothing;
    vector<ofParameter<float>> amplitudeOuts;
    vector<ofParameter<vector<float>>> fftOuts;
    
    vector<float>   ampFloatStore;
    vector<float>   oldValues;
    
    ofxAubioMelBands bands;
};

#endif /* audioAnalyzer_h */
