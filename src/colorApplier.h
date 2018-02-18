//
//  colorApplier.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 01/12/2016.
//
//

#ifndef colorApplier_h
#define colorApplier_h

#include "ofMain.h"
#include "baseIndexer.h"
#include "parametersControl.h"

class colorApplier{
    
public:
    
    colorApplier(int _id);
    ~colorApplier(){
        delete parameters;
    };
    
    
private:
    void reloadShader(bool &b);
    
    void modulationInfoListener(vector<float> &vf);
    void applyColor(ofTexture* &inputTex);
    void colorDisplacementChanged(float &f);
    void computeNewColorDisplacement(float f);
    
    void colorListener(ofColor &c);
    void colorSliderListener(int &i);
    void colorHueListener(int &i);
    
    void imageFileChangedListener(string &s);
    
    ofParameterGroup*        parameters;
    ofParameter<bool>       reloadShaderParam;
    ofParameter<ofColor>    colorPickerParam[2];
    ofParameter<int>        colorRParam[2];
    ofParameter<int>        colorGParam[2];
    ofParameter<int>        colorBParam[2];
    ofParameter<int>        colorHParam[2];
    
    ofParameter<float>  colorDisplacement;
    ofParameter<string> imageTextureFile;
//    ofParameter<int>        randomColorStepsParam;
//    ofParameter<int>        randomizeTypeColorParam; //Select the rand way: in change preset, in phasor triggered...
    
    //in and outs
    ofParameter<vector<float>>          modulationInfo[2];
    ofParameter<ofTexture*>  grayScaleIn;
    ofParameter<ofTexture*>  gradientPreview;
    ofParameter<ofTexture*>  colorizedValues;
    
    ofFbo outputFbo;
    ofFbo gradientFbo;
    ofTexture colorDisplacementTexture;
    
    bool colorIsChanging;
    ofImage imageTexture;
    bool isImageLoaded;
    float oldColorDisplacement;
    
    int width = 1;
    int height = 1;
    
    ofShader shader;
    ofFbo fbo;
    
    //TBOs
    ofTexture               modulationInfoTexture;
    ofBufferObject          modulationInfoBuffer;
};

#endif /* colorApplier_h */
