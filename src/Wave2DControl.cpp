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
    indexGrid.resize(height);
    for (auto &row : indexGrid)
        row.resize(width);
    int index_ = 0;
    for(int i=0 ; i<height ; i++){
        for(int j=0; j<width; j++){
            pair<ofVec2f, float> tempPair;
            tempPair.first = ofVec2f(j, i);
            tempPair.second = float((width*i)+j)/ float(height*width);
            indexGrid[i][j] = index_;
            barInfo_Pos.push_back(tempPair);
            index_++;
        }
    }
    
    columns_in_space = {{ofPoint(-5.885, 3.11), ofPoint(0, 3.11), ofPoint(5.885, 3.11)},
        {ofPoint(-5.885, 0), ofPoint(0, 0), ofPoint(5.885, 0)},
        {ofPoint(-5.885, -3.11), ofPoint(0, -3.11), ofPoint(5.885, -3.11)},
        {ofPoint(-5.885, -6.22), ofPoint(0, -6.22), ofPoint(5.885, -6.22)}};
    
    
    for(auto &row_column : columns_in_space){
        for(ofPoint &column : row_column){
            //center the columns
            column.y += (3.11/2);
            
            //normalize
            column /= ofPoint(5.885, 4,665);
        }
    }
        
    
    //Resize the manualOrder vector to have as many values as elements
    orderSelected.resize(height*width, 0);
    reindexSelected.resize(height*width, 0);
    
    //Add parameters in parameters group
    parameters = new ofParameterGroup;
    parameters->setName("wave2D " + ofToString(index));
    parameters->add(invert_Param.set("Invert 2D", false));
//    parameters->add(symmetryX_Param.set("SymmetryX", 0, 0, _width));
//    parameters->add(symmetryY_Param.set("SymmetryY", 0, 0, _height));
    parameters->add(phaseScale_Param.set("Phase Scale", 1, 0, 4));
    

    
    
    if(loadDropdownOptions("Functions.xml", waveFormulaOptions))
        createDropdownAndStringInput("Wave Formula", waveFormulaOptions, waveFormulaInput_Param, waveFormulaChooser_Param);
    
    parameters->add(a_Param.set("Variable a", 1, -4, 4));
    parameters->add(b_Param.set("Variable b", 1, -4, 4));
    parameters->add(c_Param.set("Variable c", 1, -4, 4));
    parameters->add(d_Param.set("Variable d", 1, -4, 4));
    
    if(loadDropdownOptions("Orders.xml", orderOptions))
        createDropdownAndStringInput("Order", orderOptions, orderInput_Param, orderChooser_Param);
    
    if(loadDropdownOptions("Reindex.xml", reindexOptions))
        createDropdownAndStringInput("Reindex", reindexOptions, reindexInput_Param, reindexChooser_Param);

    
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
    expression_parser.addSymbol("a",  *(float*)&a_Param.get());
    expression_parser.addSymbol("b", *(float*)&b_Param.get());
    expression_parser.addSymbol("c", *(float*)&c_Param.get());
    expression_parser.addSymbol("d", *(float*)&d_Param.get());
    expression_parser.registerSymbols();
    expression_parser.compileExpression(waveFormulaInput_Param);
}

vector<vector<float>> Wave2DControl::computeWave(float phasor){
    if(invert_Param) phasor = 1-phasor;
    
    vector<float> wave1d_values;
    wave1d_values.resize((width*height), 0);
    
    //manualGenerator.computeFunc(wave1d_values.data(), phasor);

    //reorder the info with the manual Order, overwrite when the value is higher, keep when it's lower;
    vector<float> wave1d_values_copy;
    wave1d_values_copy.resize(height*width, 0);
    float noValue = 0;
    for ( int i = 0; i < orderSelected.size(); i++ ) {
        float &newValue = (orderSelected[i] < 0) ? noValue : wave1d_values_copy[orderSelected[i]];
        newValue = newValue == 0 ? wave1d_values[i] : max(newValue, wave1d_values[i]);
    }
    wave1d_values = wave1d_values_copy;
    
    //iteate for all positions and calculate the function, or get the value
    for(int i = 0; i < barInfo_Pos.size() ; i++){
        auto &point = barInfo_Pos[i];
        float z;
        if(reindexSelected[i] < 0){
            z = 0;
        }else{
            auto &reindexedPoint = barInfo_Pos[reindexSelected[i]];
            if(waveFormulaOptions[waveFormulaChooser_Param] == "Manual"){
                z = wave1d_values[reindexSelected[i]];
                z = ofClamp(z, 0, 1);
            }else{
                t = 2*PI*phasor;
                ofPoint pos = columns_in_space[reindexedPoint.first.y][reindexedPoint.first.x];
                x = pos.x * PI/2;
                y = pos.y * PI/2;
                z = expression_parser.evaluateExpression();
                z = ofMap(z, -1, 1, 0, 1, true);
            }
        }
        grid[point.first.y][point.first.x] = ofClamp(z*phaseScale_Param, 0, 1);
        point.second = z;
    }
    return grid;
}

void Wave2DControl::waveFormulaDropdownListener(int &val){
    waveFormulaInput_Param = waveFormulaOptions[val];
}

void Wave2DControl::waveFormulaInputListener(string &str){
    if(str != "Manual"){
        waveFormulaChooser_Param.setWithoutEventNotifications(1);
        if(!expression_parser.compileExpression(str)) ofLog() << "Can't Compile Expression: " << str;
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
    parameters->add(tempDropdown);
    ofParameter<string> label("INSERT "+name+"_label", " ");
    parameters->add(label);
    parameters->add(textInput.set(name, options[0]));
}
