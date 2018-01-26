//
//  oscillatorTexture.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 24/01/2018.
//
//

#ifndef oscillatorTexture_h
#define oscillatorTexture_h

#include "baseIndexer.h"
#include "baseOscillator.h"

class oscillatorTexture{
public:
    oscillatorTexture(int bankId, int xSize, int ySize, ofPoint pos = ofPoint(-1, -1));
    ~oscillatorTexture(){};
    
    ofTexture&  computeBank(float phasor);
private:
    void parameterChanged(ofAbstractParameter &p);
    void reloadShader(bool &b);
    
    void newPhasorIn(float &f);
    
    void phaseOffsetListener(vector<float> &vf);
    void randomAdditionListener(vector<float> &vf);
    
    void newPowParam(float &f);
    void newpulseWidthParam(float &f);
    void newHoldTimeParam(float &f);
    
    void newQuantParam(int &i);
    void newScaleParam(float &f);
    void newOffsetParam(float &f);
    void newRandomAddParam(float &f);
    void newWaveSelectParam(int &i);
    void newAmplitudeParam(float &f);
    void newInvertParam(float &f);
    void newSkewParam(float &f);
    void newBiPowParam(float &f);
    
    vector<baseIndexer*> indexers;
    
    ofParameterGroup* parameters;
    
//    ofParameter<float>  numWaves_Param; //Desphase Quantity
//    ofParameter<float>   indexInvert_Param;
//    ofParameter<int>    symmetry_Param;
//    ofParameter<float>  indexRand_Param;
//    ofParameter<float>    indexOffset_Param;
//    ofParameter<int>    indexQuant_Param;
//    ofParameter<float>  combination_Param;
//    ofParameter<int>    modulo_Param;
//    ofParameter<bool>   manualReindex_Param;
    
    
    ofParameter<bool>       reloadShaderParam;
    ofParameter<float>    phasorIn;
    ofParameter<float>    pow_Param; //Pow of the funcion, working on sin, cos....
    ofParameter<float>    pulseWidth_Param;
    ofParameter<float>    holdTime_Param; //The duration of the hold in percentage (0.5) --> 50% of the cycle is the phase in initPhase
    ofParameter<vector<float>>   phaseOffset[2];
    ofParameter<vector<float>>   randomAddition[2];
    ofParameter<vector<float>>   scale[2];
    ofParameter<vector<float>>   offset[2];
    ofParameter<vector<float>>   pow[2];
    ofParameter<vector<float>>   bipow[2];
    ofParameter<vector<int>>   quantization[2];
    ofParameter<vector<float>>   pulseWidth[2];
    ofParameter<vector<float>>   skew[2];
    ofParameter<vector<float>>   fader[2];
    ofParameter<vector<float>>   invert[2];
    ofParameter<vector<float>>  waveform[2];
    ofParameter<int>            waveformDropdown[2];
    
    
    ofParameter<int>      quant_Param;
    ofParameter<float>    scale_Param;
    ofParameter<float>    offset_Param;
    ofParameter<float>    randomAdd_Param;
    ofParameter<float>    biPow_Param;
    ofParameter<int>      waveSelect_Param;
    ofParameter<float>    amplitude_Param;
    ofParameter<float>    invert_Param;
    ofParameter<float>    skew_Param;
    
    ofParameter<ofTexture*>      oscillatorOut;

    
    int width, height;
    
    ofShader shader;
    ofFbo   fbo;
    
    //TBOS
    //xIndex
    ofTexture               xIndexTexture;
    ofBufferObject          xIndexBuffer;
    
    //yIndex
    ofTexture               yIndexTexture;
    ofBufferObject          yIndexBuffer;
    
    //Phase Offset
    ofTexture               phaseOffsetTexture;
    ofBufferObject          phaseOffsetBuffer;
    
    //Random Addition
    ofTexture               randomAdditionTexture;
    ofBufferObject          randomAdditionBuffer;
    
};

#endif /* oscillatorTexture_h */
