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
        parameters->add(inputs[i].set("Input " + ofToString(i), nullptr));
        inputs[i].addListener(this, &textureUnifier::computeOutput);
    }
    parameters->add(output.set("Output", nullptr));
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::darkMagenta);
}

void textureUnifier::computeOutput(ofTexture* &in){
    if(&in == &inputs[triggerTextureIndex].get()){
        int totalHeight = 1;
        int maxWidth = 0;
        for(auto t : inputs){
            if(t != nullptr){
                totalHeight += t.get()->getHeight() + 1;
                if(t.get()->getWidth() > maxWidth){
                    maxWidth = t.get()->getWidth();
                }
            }
        }
        if(outputFbo.getHeight() != totalHeight || outputFbo.getWidth() != maxWidth || !outputFbo.isAllocated()){
            outputFbo.allocate(maxWidth, totalHeight);
        }
        
        outputFbo.begin();
        ofClear(0, 0, 0);
        int currentLine = 1;
        for(auto t : inputs){
            t.get()->draw(0, currentLine);
            currentLine += t.get()->getHeight() + 1;
        }
        outputFbo.end();
        
        parameters->get("Output").cast<ofTexture*>() = &outputFbo.getTexture();
    }
}
