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
    void newOrderSelected(int &val);
    void manualOrderChanged(string &str);
    
    void setPreviewTexture(bool b){previewTex = b;};
    void togglePreviewTexture(){previewTex = !previewTex;};
    
    bool loadFunctions();
    bool loadOrder();
    bool loadReindexing();
    
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
    ofParameter<int>    orderChoser_Param;
    vector<string>      formulasToChoose;
    vector<string>      orderToChoose;
    ofParameter<string> manualOrder;
    ofParameter<bool>   previewTex;
    ofParameter<int>    inversionType;
    
    vector<int>         manualOrder_int; //the order we manually type but put in a vector to be easy to use it
    
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
};


#endif /* Wave2DControl_h */
