//
//  phasorClass.cpp
//  DGTL_Generator
//
//  Created by Eduard Frigola on 28/07/16.
//
//

#include "mapper.h"
#include "parametersControl.h"


mapper::mapper(int index, ofPoint pos)
{
    parameters = new ofParameterGroup;
    parameters->setName("mapper " + ofToString(index));
  
    parameters->add(Input.set("Input", 0, 0.0, 1.0));
    parameters->add(MinInput.set("MinInput", 0, 0.0, 1.0));
    parameters->add(MaxInput.set("MaxInput", 1.0, 0.0, 1.0));
    parameters->add(MinOutput.set("MinOutput", 0, 0.0, 1.0));
    parameters->add(MaxOutput.set("MaxOutput", 1.0, 0.0, 1.0));
    parameters->add(Output.set("Output", 0, 0.0, 1.0));

    
    Input.addListener(this, &mapper::recalculate);
//    MinInput.addListener(this, &mapper::recalculate);
//    MaxInput.addListener(this, &mapper::recalculate);
//    MinOutput.addListener(this, &mapper::recalculate);
//    MaxOutput.addListener(this, &mapper::recalculate);
    
//    resetPhase_Param.addListener(this, &phasorClass::resetPhasor);
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::purple, pos);
    
    
}

mapper::~mapper(){
    delete parameters;
}

float mapper::getRange(){
//    if(!offlineMode_Param)
//        parameters->getFloat("Phasor Monitor") = ofMap(phasorMod, 0, 1, minVal_Param, maxVal_Param);
//    return (float)ofMap(phasorMod, 0, 1, minVal_Param, maxVal_Param);
    
    float f;
    recalculate(f);
    return(Output);
}




void mapper::recalculate(float& f)
{
    Output = ofMap(Input,MinInput,MaxInput,MinOutput,MaxOutput, true);
    parameters->getFloat("Output") = Output;
}
    
void mapper::resetRange()
{
    //phasor = initPhase_Param;
    //phasorMod = initPhase_Param;
    //resetPhase_Param = false;
    
    Input = 0.0;
    MinInput = 0.0;
    MaxInput = 1.0;
    MinOutput = 0.0;
    MaxOutput = 1.0;
}

