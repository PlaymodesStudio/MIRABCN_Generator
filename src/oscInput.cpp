//
//  oscInput.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 06/09/2017.
//
//

#include "oscInput.h"
#include "parametersControl.h"

oscInput::oscInput(){
    parameters = new ofParameterGroup();
    parameters->setName("oscInput");
    
    parameters->add(oscPort.set("Osc Port", "12321"));
    individualOutputs.resize(5);
    for(int i = 0 ; i < individualOutputs.size() ; i++){
        parameters->add(individualOutputs[i].set("Output " + ofToString(i), vector<float>(8)));
    }
    parameters->add(joinedOutput.set("Joined Output", vector<float>(32)));
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::aliceBlue);
    oscReceiver.setup(12321);
    
    ofAddListener(ofEvents().update, this, &oscInput::update);
}

void oscInput::update(ofEventArgs &args){
    while(oscReceiver.hasWaitingMessages()){
        ofxOscMessage m;
        oscReceiver.getNextMessage(m);
        
        string address = m.getAddress();
        vector<string>  splitAddress = ofSplitString(address, "/");
        int ovalID = ofToInt(ofToString(splitAddress[1].back()));
        int note = ofToInt(splitAddress[2]);
        string message = splitAddress.back();
        if(message == "noteOn"){
            vector<float> dupOut = individualOutputs[ovalID];
            dupOut[note] = m.getArgAsFloat(0);
            individualOutputs[ovalID] = dupOut;
            
            if(ovalID < 5){
                vector<float> dupMultiOut = joinedOutput;
                dupMultiOut[note + (8 * ovalID)] = m.getArgAsFloat(0);
                joinedOutput = dupMultiOut;
            }
            
        }else if(message == "noteOff"){
            vector<float> dupOut = individualOutputs[ovalID];
            dupOut[note] = 0;
            individualOutputs[ovalID] = dupOut;
            
            if(ovalID < 5){
                vector<float> dupMultiOut = joinedOutput;
                dupMultiOut[note + (8 * ovalID)] = 0;
                joinedOutput = dupMultiOut;
            }
        }
    }
    for(int i = 0; i < individualOutputs.size(); i++){
        int ovalID = i;
        parameters->get("Output " + ofToString(ovalID)).cast<vector<float>>() = individualOutputs[ovalID];
    }
    parameters->get("Joined Output").cast<vector<float>>() = joinedOutput;
}
