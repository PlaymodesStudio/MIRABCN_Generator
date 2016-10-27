//
//  masterControl.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 27/10/16.
//
//

#ifndef masterControl_h
#define masterControl_h

#include "ofMain.h"
#include "ofxCurvesTool.h"

class masterControl{
public:
    
    void setup(int index = 1);
    
    void computeOutTex(ofFbo &outTex, vector<float> infoVec, int i);
    void computeWaveTex(ofFbo &tex, float value, ofPoint pos);
    void computeLinWaveTex(ofFbo &tex, float value, int index);
    
    bool drawCurve(){return drawCurve_param;};
    void setCurve(ofxCurvesTool &curve){outputCurve = curve;};
    
    void setPreviewTexture(bool b){previewTex = b;};
    void togglePreviewTexture(){previewTex = !previewTex;};
    
    ofParameterGroup getParameterGroup(){return parameters;};
    
private:
    ofParameterGroup    parameters;
    ofParameter<float>  masterFader;
    ofParameter<bool>   previewTex;
    ofParameter<int>    inversionType;
    ofParameter<bool>   drawCurve_param;
    ofParameter<bool>   applyCurve_param;
    
    ofxCurvesTool       outputCurve;
};

#endif /* masterControl_h */
