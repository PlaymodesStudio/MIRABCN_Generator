//
//  delta.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 14/07/2017.
//
//

#include "delta.h"
#include "parametersControl.h"

delta::delta(int _id, ofPoint pos){
    parameters = new ofParameterGroup();
    parameters->setName("delta " + ofToString(_id));
    parameters->add(gain.set("Gain", 1, 0, 100));
    parameters->add(invert.set("Invert", false));
    parameters->add(smooth.set("Smooth", 0, 0, 1));
    parameters->add(input.set("Input", {}));
    parameters->add(output.set("Output", {}));
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::ghostWhite, pos);

    input.addListener(this, &delta::computeOutput);
}

void delta::computeOutput(vector<float> &in){
    if(outputStore.size() != in.size()){
        outputStore.resize(in.size(), 0);
    }
    if(inputStore.size() != in.size()){
        inputStore = in;
    }
    else{
        vector<float> tempOut;
        tempOut.resize(in.size(), 0);
        for(int i = 0; i < in.size(); i++){
            tempOut[i] = abs(in[i]-inputStore[i])*gain;
            tempOut[i] = (smooth*outputStore[i]) + ((1-smooth)*tempOut[i]);
        }
        inputStore = in;
        outputStore = tempOut;
        parameters->get("Output").cast<vector<float>>() = tempOut;
    }
    
}
