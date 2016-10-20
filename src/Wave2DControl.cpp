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
    manualOrder_int.resize(height*width, 0);
    
    //Add parameters in parameters group
    parameters.setName("wave2D " + ofToString(index));
    parameters.add(invert_Param.set("Invert 2D", false));
    parameters.add(symmetryX_Param.set("SymmetryX", 0, 0, _width));
    parameters.add(symmetryY_Param.set("SymmetryY", 0, 0, _height));
    parameters.add(phaseScale_Param.set("Phase Scale", 1, 0, 2));
    ofParameter<string> label("Insert Formula_label", " ");
    parameters.add(label);
    parameters.add(waveFormula_Param.set("Formula", "sin(x)"));
    ofParameterGroup formulaDropdown;
    formulaDropdown.setName("Wave Formula Select");
    //TODO: Change values
    loadFunctions();
    
    string  tempStr;
    ofParameter<string> tempStrParam("Options");
    for(auto opt : formulasToChoose)
        tempStr += opt + "-|-";
    
    tempStr.erase(tempStr.end()-3, tempStr.end());
    tempStrParam.set(tempStr);
    
//    ofParameter<string> tempStrParam("Options2", "Manual-|-sin(x)-|-cos(x)-|-sin(x+y)-|--cos(3*sqrt(pow(x,2)+pow(y,2))-t)-|-square-|-saw-|-inverted saw-|-rand1-|-rand2");
    formulaDropdown.add(tempStrParam);
    formulaDropdown.add(formulaChooser_Param.set("Wave Forumula Select", 0, 0, formulasToChoose.size()));
    parameters.add(formulaDropdown);
    
    ofParameter<string> label3("Insert Order_label", " ");
    parameters.add(label3);
    parameters.add(manualOrder.set("Manual Order", "1-2-3-4-5-6-7-8-9-10-11-12"));
    
    ofParameterGroup OrderDropdown;
    OrderDropdown.setName("Order Select");
    loadOrder();
    
    string  tempOrderStr;
    ofParameter<string> tempOrderStrParam("Options");
    for(auto opt : orderToChoose)
        tempOrderStr += opt + "-|-";
    
    tempOrderStr.erase(tempOrderStr.end()-3, tempOrderStr.end());
    tempOrderStrParam.set(tempOrderStr);
    
    OrderDropdown.add(tempOrderStrParam);
    OrderDropdown.add(orderChoser_Param.set("Order Select", 0, 0, orderToChoose.size()));
    parameters.add(OrderDropdown);
    
    //Add listeners to parameters, becouse we have to compute some things when the parameter is changed
    formulaChooser_Param.addListener(this, &Wave2DControl::newFuncSelected);
    waveFormula_Param.addListener(this, &Wave2DControl::newFuncEntered);
    orderChoser_Param.addListener(this, &Wave2DControl::newOrderSelected);
    manualOrder.addListener(this, &Wave2DControl::manualOrderChanged);
    
    //Set a default value
    manualOrder.set("1-2-3-4-5-6-7-8-9-10-11-12");


    //Add the simbols to the expression evaluator
    expression_parser.addSymbol("x", x);
    expression_parser.addSymbol("y", y);
    expression_parser.addSymbol("t", t);
    expression_parser.addSymbol("cx", cx);
    expression_parser.addSymbol("cy", cy);
    expression_parser.registerSymbols();
    expression_parser.compileExpression(waveFormula_Param);
}

vector<vector<float>> Wave2DControl::computeWave(ofFbo &waveTex, ofFbo &waveLin, float phasor){
    if(invert_Param) phasor = 1-phasor;
    
    vector<float> wave1d_values;
    wave1d_values.resize((width*height), 0);
    
    manualGenerator.computeFunc(wave1d_values.data(), phasor);

    //reorder the info with the manual Order, overwrite when the value is higher, keep when it's lower;
    vector<float> wave1d_values_copy;
    wave1d_values_copy.resize(height*width, 0);
    for ( int i = 0; i < manualOrder_int.size(); i++ ) {
        auto &newValue = wave1d_values_copy[manualOrder_int[i]];
        newValue = newValue == 0 ? wave1d_values[i] : max(newValue, wave1d_values[i]);
    }
    wave1d_values = wave1d_values_copy;
    
    //iteate for all positions and calculate the function, or get the value
    for(int i = 0; i < barInfo_Pos.size() ; i++){
        auto &point = barInfo_Pos[i];
        float z;
        if(formulasToChoose[formulaChooser_Param] == "Manual"){
            z = wave1d_values[i];
            ofClamp(z, 0, 1);
        }else{
            t = 2*PI*phasor;
            x = point.first.x;
            y = point.first.y;
            cx = point.first.x-width/2;
            cy = point.first.y-height/2+0.5;
            z = expression_parser.evaluateExpression();
            ofMap(z, -1, 1, 0, 1, true);
        }
        grid[point.first.y][point.first.x] = z*phaseScale_Param;
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
                //TODO: Change the inversion type, forula and dropdown
                if(previewTex){
                    ofSetColor(ofClamp(infoVec[j]*255, 0, 255));
                    ofDrawRectangle(i,j, 1, 1);
                    ofSetColor(ofClamp((infoVec[j])*255, 0, 255));
                    ofDrawRectangle(i+barInfo_Pos.size(),j, 1, 1);
                }else{
                    ofSetColor(infoVec[j]*255);
                    ofDrawRectangle(2*i, j,1, 1);
                    ofSetColor((infoVec[j])*255);
                    ofDrawRectangle(2*i+1, j,1, 1);
                }
            }
        }
    }
    outTex.end();
}

void Wave2DControl::newFuncSelected(int &val){
    if(formulasToChoose[formulaChooser_Param] != "Manual"){
        waveFormula_Param = formulasToChoose[val];
    }
}

void Wave2DControl::newFuncEntered(string &str){
    if(formulasToChoose[formulaChooser_Param] != "Manual"){
        expression_parser.compileExpression(str);
    }
}

void Wave2DControl::newOrderSelected(int &val){
    manualOrder = orderToChoose[val];
}

void Wave2DControl::manualOrderChanged(string &str){
    int i=0;
    for(auto num : ofSplitString(str, "-")){
        manualOrder_int[i] = ofToInt(num)-1;
        i++;
    }
}

bool Wave2DControl::loadFunctions(){
    //Test if there is no problem with the file
    ofXml xml;
    if(!xml.load("Functions.xml"))
        return false;

    for(int i=0; i<xml.getNumChildren(); i++){
        xml.setToChild(i);
        formulasToChoose.push_back(xml.getValue());
        xml.setToParent();
    }
    return true;
}

bool Wave2DControl::loadOrder(){
    ofXml xml;
    if(!xml.load("Orders.xml"))
        return false;
    
    for(int i=0; i<xml.getNumChildren(); i++){
        xml.setToChild(i);
        orderToChoose.push_back(xml.getValue());
        xml.setToParent();
    }
    return true;
}

bool Wave2DControl::loadReindexing(){
    
}