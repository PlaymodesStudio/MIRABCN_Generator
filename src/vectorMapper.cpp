//
//  vectorMapper.cpp
//  DGTL_Generator
//
//  Created by Eduard Frigola on 28/07/16.
//
//

#include "vectorMapper.h"
#include "parametersControl.h"


vectorMapper::vectorMapper(int index, ofPoint pos)
{
    parameters = new ofParameterGroup;
    parameters->setName("vectorMapper " + ofToString(index));
  
    parameters->add(Input.set("Input", {0}));
    parameters->add(MinInput.set("MinInput", 0, 0.0, 1.0));
    parameters->add(MaxInput.set("MaxInput", 1.0, 0.0, 1.0));
    parameters->add(MinOutput.set("MinOutput", 0, 0.0, 1.0));
    parameters->add(MaxOutput.set("MaxOutput", 1.0, 0.0, 1.0));
    parameters->add(Output.set("Output", {0}));

    
    Input.addListener(this, &vectorMapper::recalculate);
//    MinInput.addListener(this, &mapper::recalculate);
//    MaxInput.addListener(this, &mapper::recalculate);
//    MinOutput.addListener(this, &mapper::recalculate);
//    MaxOutput.addListener(this, &mapper::recalculate);
    
//    resetPhase_Param.addListener(this, &phasorClass::resetPhasor);
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::purple, pos);
    
    
}

vectorMapper::~vectorMapper(){
    delete parameters;
}




void vectorMapper::recalculate(vector<float>& vf)
{
    vector<float>  output;
    output.resize(vf.size(), 0);
    for(int i = 0; i < vf.size(); i++)
        output[i] = ofMap(vf[i],MinInput,MaxInput,MinOutput,MaxOutput, true);
    parameters->get("Output").cast<vector<float>>() = output;
}


