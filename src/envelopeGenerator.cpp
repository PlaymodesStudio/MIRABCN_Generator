//
//  envelopeGenerator.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 12/07/2017.
//
//

#include "envelopeGenerator.h"
#include "parametersControl.h"

envelopeGenerator::envelopeGenerator(int id, ofPoint pos){
    vector<string> easeFuncs = {"EASE_LINEAR", " EASE_IN_QUAD", " EASE_OUT_QUAD", " EASE_IN_OUT_QUAD", " EASE_IN_CUBIC", " EASE_OUT_CUBIC", " EASE_IN_OUT_CUBIC", " EASE_IN_QUART", " EASE_OUT_QUART", " EASE_IN_OUT_QUART", " EASE_IN_QUINT", " EASE_OUT_QUINT", " EASE_IN_OUT_QUINT", " EASE_IN_SINE", " EASE_OUT_SINE", " EASE_IN_OUT_SINE", " EASE_IN_EXPO", " EASE_OUT_EXPO", " EASE_IN_OUT_EXPO", " EASE_IN_CIRC", " EASE_OUT_CIRC", "EASE_IN_OUT_CIRC"};
    
    parameters = new ofParameterGroup;
    parameters->setName("envelopeGenerator " + ofToString(id));
    parameters->add(attack.set("Attack", 0, 0, 1));
    parameters->add(decay.set("Decay", 0, 0, 1));
    parameters->add(sustain.set("Sustain", 1, 0, 1));
    parameters->add(release.set("Release", 0, 0, 1));

    parametersControl::addDropdownToParameterGroupFromParameter(parameters, "Attack Ease", easeFuncs, attackCurve);
    parametersControl::addDropdownToParameterGroupFromParameter(parameters, "Decay Ease", easeFuncs, decayCurve);
    parametersControl::addDropdownToParameterGroupFromParameter(parameters, "Release Ease", easeFuncs, releaseCurve);
    
    parameters->add(gateIn.set("Gate In", {0}, {0}, {1}));
    parameters->add(output.set("Output", {0}, {0}, {1}));
    oldGateIn = {0};
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::cyan, pos);
    
    outputComputeVec = {0};
    
    gateIn.addListener(this, &envelopeGenerator::gateInChanged);
    Tweenzor::init();
    
    ofAddListener(ofEvents().update, this, &envelopeGenerator::update);
}

envelopeGenerator::~envelopeGenerator(){
    gateIn.removeListener(this, &envelopeGenerator::gateInChanged);
    ofRemoveListener(ofEvents().update, this, &envelopeGenerator::update);
    delete parameters;
}

void envelopeGenerator::update(ofEventArgs &e){
    Tweenzor::update(ofGetElapsedTimeMillis());
//    parameters->get("Output").cast<vector<float>>() = outputComputeVec;
}

void envelopeGenerator::gateInChanged(vector<float> &vf){
    if(outputComputeVec.size() != vf.size()){
        outputComputeVec.resize(vf.size(), 0);
    }else{
    
    if(vf.size() == oldGateIn.size()){
        for(int i = 0; i < vf.size(); i++){
            if(gateIn.get()[i] != oldGateIn[i]){
                if(vf[i] != 0){
                    Tweenzor::add(&outputComputeVec[i], 0.0, vf[i], 0.0, attack*5, attackCurve);
                    if(sustain != 1){
                        Tweenzor::addCompleteListener(Tweenzor::getTween(&outputComputeVec[i]), this, &envelopeGenerator::attackEnd);
                    }
                }
                else{
                    Tweenzor::add(&outputComputeVec[i], outputComputeVec[i], 0, 0.0, release*5, releaseCurve);
                }
            }
        }
        parameters->get("Output").cast<vector<float>>() = outputComputeVec;
    }
    }
    oldGateIn = vf;
}

void envelopeGenerator::attackEnd(float *f){
    for(auto &vecPos : outputComputeVec){
        if(&vecPos == f){
            if(decay == 0){
                vecPos = vecPos*sustain;
            }else{
                Tweenzor::add(&vecPos, vecPos, vecPos*sustain, 0.0, decay*5, decayCurve);
            }
        }
    }
}
