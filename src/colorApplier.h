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
    void applyColor(vector<vector<float>> &inputVec);
    void colorDisplacementChanged(float &f);
    
    void colorListener(ofColor &c);
    void colorSliderListener(int &i);
    void colorHueListener(int &i);
    
    void imageFileChangedListener(string &s);
    
    ofParameterGroup*        parameters;
    ofParameter<bool>       bypass;
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
    ofParameter<vector<float>>          indexIn;
    ofParameter<vector<vector<float>>>  grayScaleIn;
    ofParameter<vector<vector<ofColor>>>    gradientPreview;
    ofParameter<vector<vector<ofColor>>> colorizedValues;
    
    
    vector<vector<ofColor>> tempColors;
    vector<vector<ofColor>> tempGradient;
    vector<vector<float>> colorDisplacementVector;
    
    bool colorIsChanging;
    ofImage imageTexture;
    bool isImageLoaded;
    float oldColorDisplacement;
};

#endif /* colorApplier_h */
