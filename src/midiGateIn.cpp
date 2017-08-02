//
//  midiGateIn.cpp
//  MIRABCN_Generator
//
//  Created by Eduard Frigola on 14/07/2017.
//
//

#include "midiGateIn.h"
#include "parametersControl.h"

midiGateIn::midiGateIn(int _id, ofPoint pos){
    parameters = new ofParameterGroup();
    parameters->setName("midiGateIn " + ofToString(_id));
    parametersControl::addDropdownToParameterGroupFromParameter(parameters, "Midi Device", midiIn.getPortList(), midiDevice);
    parameters->add(midiChannel.set("Midi Channel", 0, 0, 16));
    parameters->add(noteOnStart.set("Note Begin", 0, 0, 127));
    parameters->add(noteOnEnd.set("Note End", 127, 0, 127));
    parameters->add(output.set("Output", {}));
    
    outputStore.resize(noteOnEnd - noteOnStart + 1, 0);
    noteOnStart.addListener(this, &midiGateIn::noteRangeChanged);
    noteOnEnd.addListener(this, &midiGateIn::noteRangeChanged);
    midiDevice.addListener(this, &midiGateIn::midiDeviceListener);
    
    parametersControl::getInstance().createGuiFromParams(parameters, ofColor::white, pos);

    ofAddListener(ofEvents().update, this, &midiGateIn::update);
}

void midiGateIn::update(ofEventArgs &e){
    Tweenzor::update(ofGetElapsedTimeMillis());
    if(mutex.try_lock()){
        parameters->get("Output").cast<vector<float>>() = outputStore;
        mutex.unlock();
    }
}

void midiGateIn::newMidiMessage(ofxMidiMessage &eventArgs){
    if(eventArgs.status == MIDI_NOTE_ON && (eventArgs.channel == midiChannel || midiChannel == 0)){
        if(eventArgs.pitch >= noteOnStart && eventArgs.pitch <= noteOnEnd){
            {
                mutex.lock();
                outputStore[eventArgs.pitch - noteOnStart] = (float)eventArgs.velocity/(float)127;
                mutex.unlock();
            }
        }
    }else if(eventArgs.status == MIDI_NOTE_OFF && (eventArgs.channel == midiChannel || midiChannel == 0)){
        if(eventArgs.pitch >= noteOnStart && eventArgs.pitch <= noteOnEnd){
            {
                mutex.lock();
                outputStore[eventArgs.pitch - noteOnStart] = 0;
                mutex.unlock();
            }
        }
    }
}

void midiGateIn::midiDeviceListener(int &device){
    midiIn.closePort();
    midiIn.openPort(device);
    midiIn.addListener(this);
}

void midiGateIn::noteRangeChanged(int &note){
    if(noteOnEnd <= noteOnStart){
        noteOnStart = 0;
    }else{
        outputStore.resize(noteOnEnd - noteOnStart + 1, 0);
    }
}
