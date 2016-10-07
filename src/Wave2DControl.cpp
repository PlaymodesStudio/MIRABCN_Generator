//
//  Wave2DControl.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 28/09/16.
//
//

#include "Wave2DControl.h"

void Wave2DControl::setup(int _width, int _height, int index){
    width = _width;
    height = _height;
    
    parameters.setName("wave2D " + ofToString(index));
    parameters.add(invert_Param.set("Invert 2D", false));
    parameters.add(symmetryX_Param.set("SymmetryX", 0, 0, _width));
    parameters.add(symmetryY_Param.set("SymmetryY", 0, 0, _height));
    parameters.add(phaseScale_Param.set("Phase Scale", 1, 0, 10));
    ofParameter<string> label("Insert Formula_label", " ");
    parameters.add(label);
    parameters.add(waveFormula_Param.set("Formula", "sin(x)"));
    ofParameterGroup formulaDropdown;
    formulaDropdown.setName("Wave Formula Select");
    //TODO: CHange values
    ofParameter<string> tempStrParam("Options2", "Manual-|-sin(x)-|-cos(x)-|-sin(x+y)-|--cos(3*sqrt(pow(x,2)+pow(y,2))-t)-|-square-|-saw-|-inverted saw-|-rand1-|-rand2");
    formulaDropdown.add(tempStrParam);
    formulaDropdown.add(formulaChooser_Param.set("Wave Forumula Select", 0, 0, ofSplitString(tempStrParam, "-|-").size()));
    parameters.add(formulaDropdown);
    
    formulaChooser_Param.addListener(this, &Wave2DControl::newFuncSelected);
    waveFormula_Param.addListener(this, &Wave2DControl::newFuncEntered);
    
    formulasToChoose = ofSplitString(tempStrParam, "-|-");
    
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
    
    expression_parser.addSymbol("x", x);
    expression_parser.addSymbol("y", y);
    expression_parser.addSymbol("t", t);
    expression_parser.addSymbol("cx", cx);
    expression_parser.addSymbol("cy", cy);
    expression_parser.registerSymbols();
    expression_parser.compileExpression(waveFormula_Param);
}

vector<vector<float>> Wave2DControl::computeWave(ofFbo &waveTex, float phasor){
    //Use the fbo to paint on it
    waveTex.begin();
    ofSetColor(0);
    for(auto point : barInfo_Pos){
        float z;
        if(formulasToChoose[formulaChooser_Param] == "Manual"){
            z = point.second;
            z += phasor;
            if(z > 1)
                z-=1;
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
        if(invert_Param){
            z *= -1;
            z += 1;
        }
        
        grid[point.first.y][point.first.x] = z*phaseScale_Param;
        point.second = z;
        ofSetColor(z*255);
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
                    ofSetColor(ofClamp((infoVec[infoVec.size()-1-j])*255, 0, 255));
                    ofDrawRectangle(i+barInfo_Pos.size(),j, 1, 1);
                }else{
                    ofSetColor(infoVec[j]*255);
                    ofDrawRectangle(2*i, j,1, 1);
                    ofSetColor((infoVec[infoVec.size()-1-j])*255);
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

