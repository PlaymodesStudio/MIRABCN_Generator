//
//  vectorChain.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 06/09/2017.
//
//

#include "vectorChain.h"
#include "parametersControl.h"

vectorChain::vectorChain(int nInputs, int id, ofPoint pos){
    if(nInputs <= 0) delete this;
    parameters = new ofParameterGroup();
    parameters->setName("vectorChain " + ofToString(id));
    
    inputs.resize(nInputs);
    for(int i = 0; i < inputs.size() ; i++){
        parameters->add(inputs[i].set("Input " + ofToString(i), {0}, {0}, {1}));
    }
    parameters->add(offset.set("Offset", 0, -1, 1));
    parameters->add(output.set("Output", {0}, {0}, {1}));
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::green, pos);
    
    inputs[0].addListener(this, &vectorChain::inputListener);
}

void vectorChain::inputListener(vector<float> &v){
    vector<float>   outChain;
    for(auto in : inputs)
        outChain.insert(outChain.end(), in.get().begin(), in.get().end());
    for(int i = 0; i < round(outChain.size()*(abs(offset))); i++){
        if(offset < 0){
            float firstValue = outChain[0];
            for(int j = 1 ; j < outChain.size() ; j++){
                outChain[j - 1] = outChain[j];
            }
            outChain.back() = firstValue;
        }
        else if(offset > 0){
            float lastValue = outChain.back();
            for(int j = outChain.size()-1 ; j > 0 ; j--){
                outChain[j] = outChain[j - 1];
            }
            outChain[0] = lastValue;
        }
    }
    parameters->get("Output").cast<vector<float>>() = outChain;
}
