//
//  Wave2DControl.h
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 28/09/16.
//
//

#ifndef Wave2DControl_h
#define Wave2DControl_h

#include "ofMain.h"
#include "ofxExprtk.h"
#include "elementOscilator.h"

class Wave2DControl{
public:
    Wave2DControl(){};
    ~Wave2DControl(){};
    
    void setup(int width, int height, int index = 0);
    vector<vector<float>> computeWave(ofFbo &waveTex, ofFbo &waveLin, float phasor);
    void computeOutTex(ofFbo &outTex, vector<float> infoVec, ofVec2f pos);
    
    void newFuncSelected(int &val);
    void newFuncEntered(string &str);
    void manualOrderChanged(string &str);
    
    void setPreviewTexture(bool b){previewTex = b;};
    void togglePreviewTexture(){previewTex = !previewTex;};
    
    ofParameterGroup getParameterGroup(){return parameters;};
    ofParameterGroup getGeneratorParameterGroup(){return manualGenerator.getParameterGroup();};
    
private:
    
    //Parameters
    ofParameterGroup    parameters;
    ofParameter<bool>   invert_Param;
    ofParameter<int>    symmetryX_Param;
    ofParameter<int>    symmetryY_Param;
    ofParameter<float>  comb_Param;
    ofParameter<float>  phaseScale_Param;
    ofParameter<string> waveFormula_Param;
    ofParameter<int>    formulaChooser_Param;
    vector<string>      formulasToChoose;
    ofParameter<string> manualOrder;
    
    vector<int>         manualOrder_int;
    
    int width;
    int height;
    
    bool previewTex = true;
    
    vector<vector<float>> grid;
    vector<pair<ofVec2f, float>> barInfo_Pos;
    
    //Buffer of intoVec, we use deque to be able to create a circular buffer, erase the old values
    deque<vector<float>> infoVecBuffer;
    
    //Function variables
    ofxExprtk<float> expression_parser;
    float x, y, t, cx, cy;
    
    
    elementOscilator manualGenerator;
};


#endif /* Wave2DControl_h */
