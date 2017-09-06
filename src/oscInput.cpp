//
//  oscInput.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 06/09/2017.
//
//

#include "oscInput.h"
#include "parametersControl.h"

oscInput::oscInput(int id, ofPoint pos){
    parameters = new ofParameterGroup();
    parameters->setName("oscInput " + ofToString(id));
    
    parameters->add(oscPort.set("Osc Port", "12321"));
    individualOutputs.resize(5);
    for(int i = 0 ; i < individualOutputs.size() ; i++){
        parameters->add(individualOutputs[i].set("Output " + ofToString(i), vector<float>(8)));
    }
    parameters->add(joinedOutput.set("Joined Output", vector<float>(40)));
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::aliceBlue, pos);
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
            parameters->get("Output " + ofToString(ovalID)).cast<vector<float>>() = dupOut;
            
            vector<float> dupMultiOut = joinedOutput;
            dupMultiOut[note + (8 * ovalID)] = m.getArgAsFloat(0);
            parameters->get("Joined Output").cast<vector<float>>() = dupMultiOut;
            
        }else if(message == "noteOff"){
            vector<float> dupOut = individualOutputs[ovalID];
            dupOut[note] = 0;
            parameters->get("Output " + ofToString(ovalID)).cast<vector<float>>() = dupOut;
            
            vector<float> dupMultiOut = joinedOutput;
            dupMultiOut[note + (8 * ovalID)] = 0;
            parameters->get("Joined Output").cast<vector<float>>() = dupMultiOut;
        }
    }
}
