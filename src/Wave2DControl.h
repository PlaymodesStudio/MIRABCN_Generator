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
#include "ofxCurvesTool.h"

class Wave2DControl{
public:
    Wave2DControl(){};
    ~Wave2DControl(){};
    
    void setup(int width, int height, int index = 0);
    vector<vector<float>> computeWave(ofFbo &waveTex, ofFbo &waveLin, float phasor);
    void computeOutTex(ofFbo &outTex, vector<float> infoVec, ofVec2f pos);
    
    void waveFormulaDropdownListener(int &val);
    void waveFormulaInputListener(string &str);
    void orderDropdownListener(int &val);
    void orderInputListener(string &str);
    void reindexDropdownListener(int &val);
    void reindexInputListener(string &str);
    
    bool drawCurve(){return drawCurve_param;};
    void setCurve(ofxCurvesTool curve){outputCurve = curve;};
    
    void setPreviewTexture(bool b){previewTex = b;};
    void togglePreviewTexture(){previewTex = !previewTex;};
    
    bool loadDropdownOptions(string filename, vector<string> &options);
    
    ofParameterGroup getParameterGroup(){return parameters;};
    ofParameterGroup getGeneratorParameterGroup(){return manualGenerator.getParameterGroup();};
    
    void createDropdownAndStringInput(string name, vector<string> options, ofParameter<string> textInput, ofParameter<int> dropDownSelector);
    
private:
    
    //Parameters
    ofParameterGroup    parameters;
    ofParameter<bool>   invert_Param;
    ofParameter<int>    symmetryX_Param;
    ofParameter<int>    symmetryY_Param;
    ofParameter<float>  comb_Param;
    ofParameter<float>  phaseScale_Param;
    
    ofParameter<string> waveFormulaInput_Param;
    ofParameter<int>    waveFormulaChooser_Param;
    vector<string>      waveFormulaOptions;
    
    ofParameter<string> orderInput_Param;
    ofParameter<int>    orderChooser_Param;
    vector<string>      orderOptions;
    vector<int>         orderSelected; //the order we manually type but put in a vector to be easy to use it
    
    ofParameter<string> reindexInput_Param;
    ofParameter<int>    reindexChooser_Param;
    vector<string>      reindexOptions;
    vector<int>         reindexSelected;
    
    ofParameter<bool>   previewTex;
    ofParameter<int>    inversionType;
    ofParameter<bool>   drawCurve_param;
    ofParameter<bool>   applyCurve_param;
    
    
    
    int width;
    int height;
    
    //Boolean for changing the way we see (it outputs) the output texture
    
    //Here there is a odd thing, we have the same values but represented in diferent ways
    //Grid way is more clarifying
    vector<vector<float>> grid;
    //linear way is used to be able to fast get the columns in the order we first specyfied, the pair contains, first the position, then the value in this pos
    vector<pair<ofVec2f, float>> barInfo_Pos;

    
    //Variables for the function expression evaluator
    ofxExprtk<float> expression_parser;
    float x, y, t, cx, cy;
    
    //We insert a oscillator when we use the mode 2D for wave generation
    //TODO: Find a better way, define the generator outside, and pass it as a variable. Or redo the computeFunc function so from outside can be called
    elementOscilator manualGenerator;
    
    ofxCurvesTool outputCurve;
};


#endif /* Wave2DControl_h */
