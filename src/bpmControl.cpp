//
//  bpmControl.cpp
//  DGTL_Generator
//
//  Created by Eduard Frigola on 12/08/16.
//
//

#include "bpmControl.h"

void bpmControl::setup(){
    // setup beat object
    beat.setup();
    //beat.setup("default", 2 * bufferSize, bufferSize, samplerate);
    
    soundStream.setup(0, 2, 44100, 512, 4);
    soundStream.setInput(this);
    //ofSoundStreamSetup(nOutputs, nInputs, sampleRate, bufferSize, nBuffers);
    //ofSoundStreamListDevices();
}

void bpmControl::audioIn(float * input, int bufferSize, int nChannels){
    // compute beat location
    beat.audioIn(input, bufferSize, nChannels);
}
