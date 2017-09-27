//
//  textureUnifier.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 22/09/2017.
//
//

#include "textureUnifier.h"
#include "parametersControl.h"

textureUnifier::textureUnifier(int numInputs){
    parameters = new ofParameterGroup();
    parameters->setName("textureUnifier");
    
    parameters->add(triggerTextureIndex.set("Trigger Index", 0, 0, numInputs - 1));
    inputs.resize(numInputs);
    for(int i = 0; i < inputs.size() ; i++){
        parameters->add(inputs[i].set("Input " + ofToString(i), {{}}));
        inputs[i].addListener(this, &textureUnifier::computeOutput);
    }
    parameters->add(output.set("Output", {{}}));
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::darkMagenta);
}

void textureUnifier::computeOutput(vector<vector<ofColor> > &in){
    if(&in == &inputs[triggerTextureIndex].get()){;
        
        vector<vector<ofColor>> outInfo;
        int width = 0;
        for (int i = 0; i < inputs.size(); i++){
            if(inputs[i].get()[0].size() > width) width = inputs[i].get()[0].size();
        }
        int height = 1;
        for (int i = 0; i < inputs.size(); i++){
            height += inputs[i].get().size()+1;
        }
        outInfo.resize(height, vector<ofColor>(width, ofColor::black));
        
        
        
        int currentLine = 1;
        
        for (int i = 0; i < inputs.size(); i++){
            for(int j = 0; j < inputs[i].get().size(); j++){
                for(int k = 0; k < inputs[i].get()[j].size(); k++){
                    outInfo[currentLine][k] =  inputs[i].get()[j][k];
                }
                currentLine++;
            }
            currentLine++;
        }
        

        
        parameters->get("Output").cast<vector<vector<ofColor>>>() = outInfo;
    }
}
