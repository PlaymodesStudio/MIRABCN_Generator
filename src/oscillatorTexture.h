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
    void scaleListener(vector<float> &vf);
    void offsetListener(vector<float> &vf);
    void powListener(vector<float> &vf);
    void bipowListener(vector<float> &vf);
    void quantizationListener(vector<int> &vf);
    void pulseWidthListener(vector<float> &vf);
    void skewListener(vector<float> &vf);
    void faderListener(vector<float> &vf);
    void invertListener(vector<float> &vf);
    void waveformListener(vector<float> &vf);
    void newWaveSelectParam(int &i);
    
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
    
    ofParameter<int>      waveSelect_Param;

    
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
    
    //Scale
    ofTexture               scaleTexture;
    ofBufferObject          scaleBuffer;
    
    //Offset
    ofTexture               offsetTexture;
    ofBufferObject          offsetBuffer;
    
    //Pow
    ofTexture               powTexture;
    ofBufferObject          powBuffer;
    
    //Bipow
    ofTexture               bipowTexture;
    ofBufferObject          bipowBuffer;
    
    //Quantization
    ofTexture               quantizationTexture;
    ofBufferObject          quantizationBuffer;
    
    //Pulse Width
    ofTexture               pulseWidthTexture;
    ofBufferObject          pulseWidthBuffer;
    
    //Skew
    ofTexture               skewTexture;
    ofBufferObject          skewBuffer;
    
    //Fader
    ofTexture               faderTexture;
    ofBufferObject          faderBuffer;
    
    //Invert
    ofTexture               invertTexture;
    ofBufferObject          invertBuffer;
    
    //Waveform
    ofTexture               waveformTexture;
    ofBufferObject          waveformBuffer;
    
    
};

#endif /* oscillatorTexture_h */
