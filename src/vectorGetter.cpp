//
//  vectorGetter.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 06/09/2017.
//
//

#include "vectorGetter.h"
#include "parametersControl.h"

vectorGetter::vectorGetter(int id, ofPoint pos){
    parameters = new ofParameterGroup();
    parameters->setName("vectorGetter " + ofToString(id));
    
    parameters->add(input.set("Input", {}));
    parameters->add(index.set("Index", "0"));
    parameters->add(output.set("Output", 0));
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::green, pos);
    
    input.addListener(this, &vectorGetter::inputListener);
}

void vectorGetter::inputListener(vector<float> &v){
    if(input.get().size() > ofToInt(index)){
        parameters->getFloat("Output") = input.get()[ofToInt(index)];
    }
}
