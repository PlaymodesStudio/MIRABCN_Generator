//
//  Wave2DControl.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 28/09/16.
//
//

#include "Wave2DControl.h"

void Wave2DControl::setup(int _width, int _height, int index){
    //Assign de size of the grid
    width = _width;
    height = _height;
    
    //Create the wave Generator for the Wave in linear mode
    manualGenerator.setIndexCount(width*height);
    manualGenerator.setup(2);
    
    
    //Resize height and barInfo_pos
    grid.resize(height);
    for( auto &row : grid)
        row.resize(width, 0);
    for(int i=0 ; i<height ; i++){
        for(int j=0; j<width; j++){
            pair<ofVec2f, float> tempPair;
            tempPair.first = ofVec2f(j, i);
            tempPair.second = float((width*i)+j)/ float(height*width);
            barInfo_Pos.push_back(tempPair);
        }
    }
    
    //Resize the manualOrder vector to have as many values as elements
    orderSelected.resize(height*width, 0);
    reindexSelected.resize(height*width, 0);
    
    //Add parameters in parameters group
    parameters.setName("wave2D " + ofToString(index));
    parameters.add(invert_Param.set("Invert 2D", false));
//    parameters.add(symmetryX_Param.set("SymmetryX", 0, 0, _width));
//    parameters.add(symmetryY_Param.set("SymmetryY", 0, 0, _height));
    parameters.add(phaseScale_Param.set("Phase Scale", 1, 0, 4));
    


    if(loadDropdownOptions("Functions.xml", waveFormulaOptions))
        createDropdownAndStringInput("Wave Formula", waveFormulaOptions, waveFormulaInput_Param, waveFormulaChooser_Param);
    
    if(loadDropdownOptions("Orders.xml", orderOptions))
        createDropdownAndStringInput("Order", orderOptions, orderInput_Param, orderChooser_Param);
    
    if(loadDropdownOptions("Reindex.xml", reindexOptions))
        createDropdownAndStringInput("Reindex", reindexOptions, reindexInput_Param, reindexChooser_Param);

    
    
    parameters.add(previewTex.set("Preview Texutre", 0));
    
    ofParameterGroup invertDropDown;
    invertDropDown.setName("Inversion Type");
    ofParameter<string> tempInvStrParam("Options", "no-|-position inverse-|-value inverse");
    invertDropDown.add(tempInvStrParam);
    invertDropDown.add(inversionType.set("Inversion Type", 0, 0, 3));
    parameters.add(invertDropDown);
    
    
    
    parameters.add(drawCurve_param.set("Draw Curve", 0));
    parameters.add(applyCurve_param.set("Apply Curve", 0));
    
    //Add listeners to parameters, becouse we have to compute some things when the parameter is changed
    waveFormulaChooser_Param.addListener(this, &Wave2DControl::waveFormulaDropdownListener);
    waveFormulaInput_Param.addListener(this, &Wave2DControl::waveFormulaInputListener);
    orderChooser_Param.addListener(this, &Wave2DControl::orderDropdownListener);
    orderInput_Param.addListener(this, &Wave2DControl::orderInputListener);
    reindexChooser_Param.addListener(this, &Wave2DControl::reindexDropdownListener);
    reindexInput_Param.addListener(this, &Wave2DControl::reindexInputListener);
    
    //Set a default value
    orderInput_Param.set("1-2-3-4-5-6-7-8-9-10-11-12");
    reindexInput_Param.set("1-2-3-4-5-6-7-8-9-10-11-12");


    //Add the simbols to the expression evaluator
    expression_parser.addSymbol("x", x);
    expression_parser.addSymbol("y", y);
    expression_parser.addSymbol("t", t);
    expression_parser.addSymbol("cx", cx);
    expression_parser.addSymbol("cy", cy);
    expression_parser.registerSymbols();
    expression_parser.compileExpression(waveFormulaInput_Param);
}

vector<vector<float>> Wave2DControl::computeWave(ofFbo &waveTex, ofFbo &waveLin, float phasor){
    if(invert_Param) phasor = 1-phasor;
    
    vector<float> wave1d_values;
    wave1d_values.resize((width*height), 0);
    
    manualGenerator.computeFunc(wave1d_values.data(), phasor);

    //reorder the info with the manual Order, overwrite when the value is higher, keep when it's lower;
    vector<float> wave1d_values_copy;
    wave1d_values_copy.resize(height*width, 0);
    for ( int i = 0; i < orderSelected.size(); i++ ) {
        auto &newValue = wave1d_values_copy[orderSelected[i]];
        newValue = newValue == 0 ? wave1d_values[i] : max(newValue, wave1d_values[i]);
    }
    wave1d_values = wave1d_values_copy;
    
    //iteate for all positions and calculate the function, or get the value
    for(int i = 0; i < barInfo_Pos.size() ; i++){
        auto &point = barInfo_Pos[i];
        auto &reindexedPoint = barInfo_Pos[reindexSelected[i]];
        float z;
        if(waveFormulaOptions[waveFormulaChooser_Param] == "Manual"){
            z = wave1d_values[reindexSelected[i]];
            z = ofClamp(z, 0, 1);
        }else{
            t = 2*PI*phasor;
            x = reindexedPoint.first.x;
            y = reindexedPoint.first.y;
            cx = reindexedPoint.first.x-width/2;
            cy = reindexedPoint.first.y-height/2+0.5;
            z = expression_parser.evaluateExpression();
            z = ofMap(z, -1, 1, 0, 1, true);
        }
        grid[point.first.y][point.first.x] = ofClamp(z*phaseScale_Param, 0, 1);
        point.second = z;
    }
    
    //Draw info to the FBO's
    waveLin.begin();
    ofSetColor(0);
    ofDrawRectangle(0, 0, waveLin.getWidth(), waveLin.getHeight());
    //Draw the Bars
    float wid = 1;
    float hei = waveLin.getHeight();
    ofSetColor(255);
    for(int i = 0; i < width*height; i++)
        ofDrawRectangle((i*wid), (1-barInfo_Pos[i].second)*hei, wid, barInfo_Pos[i].second*hei);
    waveLin.end();
    
    
    waveTex.begin();
    ofSetColor(0);
    for(auto point : barInfo_Pos){
        ofSetColor(point.second*255);
        ofDrawRectangle(point.first.x, point.first.y, 1, 1);
    }
    waveTex.end();
    return grid;
}

void Wave2DControl::computeOutTex(ofFbo &outTex, vector<float> infoVec, ofVec2f pos){
    outTex.begin();
    for(int i=0 ; i < barInfo_Pos.size() ; i++){
        auto &point = barInfo_Pos[i];
        if(point.first.x == pos.x && point.first.y == pos.y){
            for(int j = 0; j < infoVec.size(); j++){
                int valueInByte = infoVec[j]*255;
                if(applyCurve_param)
                    valueInByte = outputCurve[valueInByte];
                
                ofSetColor(valueInByte);
                if(previewTex){
                    ofDrawRectangle(i,j, 1, 1);
                }else{
                    ofDrawRectangle(2*i,j, 1, 1);
                }
            }
            for(int j = 0; j < infoVec.size(); j++){
                int valueInByte;
                switch (inversionType) {
                    case 0:
                        valueInByte = infoVec[j]*255;
                        break;
                        
                    case 1:
                        valueInByte = infoVec[infoVec.size()-1-j]*255;
                        break;
                        
                    case 2:
                        valueInByte = (1-infoVec[j])*255;
                        break;
                        
                    default:
                        valueInByte = infoVec[j]*255;
                        break;
                }
                if(applyCurve_param)
                    valueInByte = outputCurve[valueInByte];
                
                ofSetColor(valueInByte);
                
                if(previewTex){
                    ofDrawRectangle(i+barInfo_Pos.size(),j, 1, 1);
                }else{
                    ofDrawRectangle(2*i+1,j, 1, 1);
                }
                
            }
        }
    }
    outTex.end();
}

void Wave2DControl::waveFormulaDropdownListener(int &val){
    if(waveFormulaOptions[waveFormulaChooser_Param] != "Manual"){
        waveFormulaInput_Param = waveFormulaOptions[val];
    }
}

void Wave2DControl::waveFormulaInputListener(string &str){
    if(waveFormulaOptions[waveFormulaChooser_Param] != "Manual"){
        expression_parser.compileExpression(str);
    }
}

void Wave2DControl::orderDropdownListener(int &val){
    orderInput_Param = orderOptions[val];
}

void Wave2DControl::orderInputListener(string &str){
    int i=0;
    for(auto num : ofSplitString(str, "-")){
        orderSelected[i] = ofToInt(num)-1;
        i++;
    }
}

void Wave2DControl::reindexDropdownListener(int &val){
    reindexInput_Param = reindexOptions[val];
}

void Wave2DControl::reindexInputListener(string &str){
    int i=0;
    for(auto num : ofSplitString(str, "-")){
        reindexSelected[i] = ofToInt(num)-1;
        i++;
    }
}

bool Wave2DControl::loadDropdownOptions(string filename, vector<string> &options){
    ofXml xml;
    if(!xml.load(filename))
        return false;
    
    for(int i=0; i<xml.getNumChildren(); i++){
        xml.setToChild(i);
        options.push_back(xml.getValue());
        xml.setToParent();
    }
    return true;
}


void Wave2DControl::createDropdownAndStringInput(string name, vector<string> options, ofParameter<string> textInput, ofParameter<int> dropDownSelector){
    ofParameterGroup tempDropdown;
    tempDropdown.setName(name + " Select");
    string  tempStr;
    ofParameter<string> tempStrParam("Options");
    for(auto opt : options)
        tempStr += opt + "-|-";
    
    tempStr.erase(tempStr.end()-3, tempStr.end());
    tempStrParam.set(tempStr);
    

    tempDropdown.add(tempStrParam);
    tempDropdown.add(dropDownSelector.set(name +" Select", 0, 0, options.size()));
    parameters.add(tempDropdown);
    ofParameter<string> label("Insert "+name+"_label", " ");
    parameters.add(label);
    parameters.add(textInput.set(name, options[0]));
}